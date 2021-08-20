from torch import nn
from .BasicModule import BasicModule

class PYNQ_Model(BasicModule):
    def __init__(self):
        super(PYNQ_Model,self).__init__()
        self.model_name = str(type(self))
        
        self.features = nn.Sequential(
                        nn.Conv2d(1, 4, kernel_size = 5, stride = 1, padding = 0),
                        nn.ReLU(inplace = True),
                        nn.MaxPool2d(kernel_size = 2, stride = 2),
                        nn.Conv2d(4, 8, kernel_size = 5, stride = 1, padding = 0),
                        nn.ReLU(inplace = True),
                        nn.MaxPool2d(kernel_size = 2, stride = 2),
                        nn.Conv2d(8, 16, kernel_size = 5, stride = 1, padding = 0),
                        nn.ReLU(inplace = True),
                        nn.MaxPool2d(kernel_size = 2, stride = 2),
        )
        
        self.calssifier = nn.Sequential(
                          nn.Linear(27*27*16,1024),
                          nn.ReLU(inplace = True),
                          nn.Linear(1024,2),
                          nn.ReLU(inplace = True),
        )
        
    def forward(self,x):
        x = self.features(x)
        x = x.view(x.size(0),256 * 6 * 6) # because the picture 's dim = 3 ?
        x = x.calssifier(x)
        return x