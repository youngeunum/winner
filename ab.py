caffe_root = '/home/socmgr/caffe/'

import sys
import json
sys.path.insert(0, caffe_root + 'python')
import caffe

prototxt = '/home/socmgr/caffe/models/SqueezeNet/SqueezeNet_v1.0/deploy.prototxt'
caffemodel = '/home/socmgr/caffe/models/SqueezeNet/SqueezeNet_v1.0/squeezenet_v1.0.caffemodel'

dict_params = {}
net = caffe.Net(prototxt,caffemodel, caffe.TEST)
for param in net.params:
	dict_params[param] = {'data':net.params[param][0].data.tolist(),
			      'shape': net.params[param][0].data.shape}

with open('weights.json', 'w') as outfile:
	json.dump(dict_params, outfile)
