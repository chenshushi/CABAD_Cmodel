import time
class Default_cfg(object):
    model       = 'PYNQ_net'   # the name of your model, should keepace with medel/__init__.py
    batch_size  = 4
    lr          = 0.01
    max_epoch   = 10
    load_model_path = ''
    train_data_root = ''
    use_gpu     = True
    test_data   = ''
    batch_size  = 128
    num_workers = 4
    weight_decay= 0.1
    print_freq = 20 # print info every N batch
    lr_decay   = 0.01
    def __parse__ (self,kwargs):
        '''
           update the config parameter accroding to kwags
        '''
        for k,v in kwargs.items():
            if not hasattr(self,k):
                print('sorry, do not have the parameter %s' % k)
            else:
                setattr(self,k,v)
        print('you have successfully set this parameter:\n')
        for k,v in self.__class__.__dict__.items():
            if not k.startswith('__'):
                print(k,getattr(self,k),sep = '\t\t\t')
            #print("K : %s" % k, "V : %s" % v, sep = '\t\t\t\t')

cfg = Default_cfg()