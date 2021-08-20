import fire
from config import cfg
import models
from torch.utils.data import DataLoader
from data.dataset import DogCat
def train (**kwargs):
    cfg.__parse__(kwargs)

    # get model
    model = getattr(models,cfg.model)().eavl()
    if cfg.load_model_path:
        model.load(cfg.load_model_path)
    if cfg.use_gpu: model.cuda()

    # data
    train_data = DogCat(cfg.test_data,test=True)
    test_dataloader = DataLoader(train_data,batch_size=opt.batch_size,shuffle=False,num_workers=opt.num_workers)
    results = []





if __name__ == '__main__':
    fire.Fire()