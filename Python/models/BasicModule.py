import torch as t
import time
class BasicModule(t.nn.Module):
    def __init__(self):
        super(BasicModule, self).__init__()
        self.model_name = str(type(self))
    
    def load(self,path):
        self.load_state_dict(t.load(path))
    def save(self,PATH = None):
        if PATH is None:
            prefix = 'checkpoint/' + self.model_name + '_'
            PATH = time.strftime(prefix + '%m%d_%H%M%S.pth')
        t.save(self.state_dict(),PATH)
        return PATH


class Flat(t.nn.Module):
    def __init__(self):
        super(Flat,self).__init__()
    def forward(self,x):
        return x.view(x.size(0),-1)