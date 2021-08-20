from torch import nn
from .BasicModule import BasicMoudle

class PYNQ_net (BasicMoudle):
    def __init__(self,num_calss = 2):
        super(PYNQ_net,self).__init__()
        self.model_name = 'PYNQ_net'

        self.features = nn.Sequential(
            nn.Conv2d(1, 4, kernel_size=5, stride=1, padding=0),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(kernel_size=2, stride=2),
            nn.Conv2d(4, 8, kernel_size=5, stride=1, padding=0),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(kernel_size=2, stride=2),
            nn.Conv2d(8, 16, kernel_size=5, stride=1, padding=0),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(kernel_size=2, stride=2),
        )
        self.classifier = nn.Sequential(
            nn.Linear(27*27*16,1024),
            nn.ReLU(inplace=True),
            nn.Linear(1024,num_calss),
        )
    def forward(self,x):
        x = self.features(x)
        x = x.view(x.size(0),27*27*16)
        x = self.calssifier(x)
        return x