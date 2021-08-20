from config import cfg
import os
import torch as t
import models
from data.dataset import DogCat
from torch.utils.data import DataLoader
from torch.autograd import Variable
from torchnet import meter
from utils.visualize import Visualizer
from tqdm import tqdm

def train(**kwargs):
    cfg.__parse__(kwargs)
    vis = Visualizer(cfg.env)
    # model
    model = getattr(models,cfg.model).eval()
    if cfg.load_model_path:
        model.load(cfg.load_model_path)
    if cfg.use_gpu:
        model.cuda()
    
    # data
    train_data          = DogCat(cfg.train_data_root, train = True , test = False, val = False)
    val_data            = DogCat(cfg.train_data_root, train = False, test = False, val = True )
    train_dataLoader    = DataLoader(train_data, batch_size = cfg.batch_size, shuffle = False, num_workers = cfg.num_workers)
    val_dataLoader      = DataLoader(val_data  , batch_size = cfg.batch_size, shuffle = False, num_workers = cfg.num_workers)
    
    # criterion and optimizer
    criterion = t.nn.CrossEntropyLoss()
    optimizer = t.optim.Adam()
    lr        = cfg.lr
    
    # visualization
    loss_meter = meter.AverageValueMeter()
    confusion_matrix = meter.ConfusionMeter(2)
    previous_loss = 1e100
    
    #train
    for epoch in range(cfg.max_epoch):
        loss_meter.reset()
        confusion_matrix.reset()
        
        for i,(data,label) in tqdm(enumerate(train_dataLoader), total = len(train_dataLoader)):
            input  = Variable(data)
            target = Variable(label)
            if cfg.use_gpu:
                input = input.cuda()
                target = target.cuda()
            optimizer.zero_grad()
            score = model(input)
            loss = criterion(score,target)
            loss.backward()
            optimizer.step()

            # meters update and visualize
            loss_meter.add(loss.data)
            confusion_matrix.add(score.data, target.data)

            if i%cfg.print_freq == cfg.print_freq:
                vis.plot('loss',loss_meter.value()[0])
            
            model.save()

            # validate and visualze
            val_cm,val_accuracy = val(model,val_dataLoader)
            vis.plot('val_accuracy',val_accuracy)
            vis.log("epoch:{epoch},lr:{lr},loss:{loss},train_cm:{train_cm},val_cm:{val_cm}".format(
                    epoch = epoch,loss = loss_meter.value()[0],val_cm = str(val_cm.value()),train_cm=str(confusion_matrix.value()),lr=lr))

            if loss_meter.value()[0] > previous_loss:
                lr = lr * cfg.lr_decay
                for param_group in optimizer.param_groups:
                    param_group['lr'] = lr

            previous_loss = loss_meter.value()[0]
def val(model,dataloader):
    model.eval()

    confusion_matrix = meter.ConfusionMeter(2)
    for ii,(data, label) in enumerate(dataloader):
        input  = Variable(data,  volatile = True)
        target = Variable(label, volatile = True)
        if cfg.use_gpu:
            input  = input.cuda()
            target = target.cuda()
        score = model(input)
        confusion_matrix.add(score.data.squeeze(), label)
        model.train()

        cm_value = confusion_matrix.value()
        accuracy = 100. * (cm_value[0][0] + cm_value[1][1]) / (cm_value.sum())
        return confusion_matrix, accuracy

def test(**kwargs):
    cfg.__parse__(kwargs)

    # configure model
    model = getattr(models, cfg.model).eval()
    if cfg.load_model_path:
        model.load(cfg.load_model_path)
    if cfg.use_gpu:
        model.cuda()
    
    #data
    train_data = DogCat(cfg.test_data_root,test = True)
    test_dataloader = DataLoader(train_data, batch_size = cfg.batch_size, shuffle = False, num_workers = opt.num_workers)
    results = []
    for ii,(data,path) in enumerate(test_dataloader):
        input = t.autograd.Variable(data,volatile = True)
        if cfg.use_gpu: input = input.cuda()
        score = model(input)
        probability = t.nn.functional.softmax(score)[:,0].data.tolist()
        # label = score.max(dim = 1)[1].data.tolist()
        
        batch_results = [(path_,probability_) for path_,probability_ in zip(path,probability) ]

        results += batch_results
    write_csv(results,cfg.result_file)

    return results

def write_csv(results,file_name):
    import csv
    with open(file_name,'w') as f:
        writer = csv.writer(f)
        writer.writerow(['id','label'])
        writer.writerows(results)



if __name__ == '__main__':
    import fire
    fire.Fire()