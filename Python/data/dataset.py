import os
from PIL import Image
from torch.utils import data
import numpy as np
from torchvision import transforms as T

class DogCat(data.Dataset):
    def __init__(self,root, train = False, test = False, val = False, transforms = None):
        imgs = [os.path.join(root,img) for img in os.listdir(root)]
        self.test = test
        self.train = train
        self.val = val

        if train and not test and not val:
            imgs = sorted(imgs,key = lambda x : int(x.split(".")[-2]))
        elif not train and test and not val:
            imgs = sorted(imgs,key=lambda x: int(x.split('.')[-2].split('\\')[-1]))
        elif not train and not test and val:
            imgs = sorted(imgs,key = lambda x : int(x.split(".")[-2]))
        else:
            print("the values of train, test and val are wrong!\n")
        
        # shuffle
        np.random.seed(100)
        imgs = np.random.permutation(imgs)

        # make dataset
        imgs_num = len(imgs)
        if not train and test and not val:
            self.imgs = imgs
        elif train and not test and not val:
            self.imgs = imgs[:int(0.7*imgs_num)]
        elif not train and not test and val:
            self.imgs = imgs[int(0.7*imgs_num):]

        # transform the imgs data
        if transforms is not None:
            normalize = T.Normalize(mean = [0.485, 0.456, 0.406],
                                     std = [0.229, 0.224, 0.225])
            if test or not train:
                self.transforms = T.Compose([T.Scale(224),
                                             T.CenterCrop(224),
                                             T.ToTensor(),
                                             normalize])
            else:
                self.transforms = T.Compose([T.Scale(256),
                                             T.RandomSizedCrop(224),
                                             T.RandomHorizontalFlip(),
                                             T.ToTensor(),
                                             normalize])

    # rewrite function gettitem and len
    def __getitem__(self,index):
        """return picture data"""
        img_path = self.imgs[index]
        if self.test:
            label = int(self.imgs[index].split('.')[-2].split('\\')[-1])
        else:
            if 'dog' in self.imgs[index].split('\\')[-1].split('.')[-3]:
                label = 1
            else:
                label = 0
        data = Image.open(self.imgs[index])
        data = self.transforms(data)
        return data, label
    def __len__(self):
        return len(self.imgs)