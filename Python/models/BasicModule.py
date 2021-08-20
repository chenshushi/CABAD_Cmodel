import torch as t
import time
class BasicModule(t.nn.Module):
    '''
    provide load and save function
    '''
    def __init__(self):
        super(BasicModule,self).__init__()
        self.model_name=type(self)
    def load (self,model_path):
        self.load_state_dict(t.load(model_path))
    def save(self, name=None):
        '''
        save the model , use the model's name + time as file name
        '''
        if name is None:
            prefix = 'checkpointd/' + self.model_name + '_'
            name   = time.strftime(prefix + '%m%d_%H:%M:%S.pth')
        t.save(self.state_dict(),name)
        return name
class Flat(t.nn.Module):
    def __init__(self):
        super(Flat,self).__init__()
    def forward(self,x):
        return x.view(x.size(0),-1)