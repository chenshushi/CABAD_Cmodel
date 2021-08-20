import fire
from config import cfg
import torch as t
import models
from torch.utils.data import DataLoader
from torch.autograd import Variable
from data.dataset import DogCat
from tqdm import tqdm
from torchnet import meter
def test (**kwargs):
    cfg.__parse__(kwargs)

    # get model
    model = getattr(models,cfg.model)().eavl()
    if cfg.load_model_path:
        model.load(cfg.load_model_path)
    if cfg.use_gpu: model.cuda()

    # data
    test_data = DogCat(cfg.test_data,test=True)
    test_dataloader = DataLoader(test_data,batch_size=cfg.batch_size,shuffle=False,num_workers=cfg.num_workers)
    results = []
    for ii, (data,path) in enumerate(test_dataloader):
        input = t.autograd.variable(data,volatile = True)
        if cfg.use_gpu:
            input = input.cuda()
        score = model(input)
        probability = t.nn.functional.softmax(score)[:,0].data.tolist()
        batch_results = [(path_,probability_) for path_,probability_ in zip(path, probability)]

        results += batch_results
    write_csv(results,cfg.result_file)

    return results

def write_csv(results,file_name):
    import csv
    with open(file_name,'w') as f:
        writer = csv.writer(f)
        writer.writerow(['id','lable'])
        writer.writerows(results)

def train(**kwargs):
    cfg.__parse__(kwargs)
    vis = Visualizer(opt.env)
    # config model
    model = getattr(models,cfg.model)()
    if cfg.load_model_path:
        model.load(cfg.load_model_path)
    if cfg.use_gpu:
        model.cuda()
    
    # data
    train_data = DogCat(cfg.train_data_root, train = True)
    val_data   = DogCat(cfg.train_data_root, train = False)
    train_dataloader = DataLoader(train_data,cfg.batch_size, shuffle = True, num_workers = cfg.num_workers)
    val_dataloader   = DataLoader(val_data,  cfg.batch_size, shuffle = True, num_workers = cfg.num_workers)

    # criterion and optimizer
    criterion = t.nn.CrossEntropyLoss()
    lr = cfg.lr
    optimizer = t.optim.Adam(model.Parameters(),lr = lr, weight_decay = cfg.weight_decay)

    # meters
    loss_meter = meter.AverageValueMeter()
    confusion_materix = meter.ConfusionMeter(2)
    previous_loss = 1e100
    # train
    for epoch in range(cfg.max_epoch):
        #loss_meter.reset()
        #confusion_matrix.reset()

        for ii,(data,label) in tqdm(enumerate(train_dataloader),total=len(train_data)):
            # train model
            input  = Variable(data)
            traget = Variable(label)
            if cfg.use_gpu:
                input = input.cuda()
                target = target.cuda()
            optimizer.zero_grad()
            score = model(input)
            loss = criterion(score,target)
            loss.backward()
            optimizer.step()

            # meter update and visualize
            loss_meter.add(loss.data[0])
            confusion_matrix.add(score.data, target.data)

            if ii%cfg.print_freq==cfg.print_freq-1:
                vis.plot('loss', loss_meter.value()[0])

    # save mdel
    model.save()
    
    # validate and visualize
    val_cm,val_accuracy = val(model,val_dataloader)
    
    vis.plot('val_accuracy',val_accuracy)
    vis.log("epoch:{epoch},lr:{lr},loss:{loss},train_cm:{train_cm},val_cm:{val_cm}".format(
                epoch = epoch,loss = loss_meter.value()[0],val_cm = str(val_cm.value()),train_cm=str(confusion_matrix.value()),lr=lr))
    # update learning rate
    if loss_meter.value()[0] > previous_loss:          
        lr = lr * cfg.lr_decay
        # 第二种降低学习率的方法:不会有moment等信息的丢失
        #for param_group in optimizer.param_groups:
        #    param_group['lr'] = lr
        


def val(model,dataloader):

    model.eval()
    confusion_matrix = meter.ConfusionMeter(2)
    for ii, data in enumerate(dataloader):
        input, label = data
        val_input = Variable(input, volatile=True)
        val_label = Variable(label.type(t.LongTensor), volatile=True)
        if cfg.use_gpu:
            val_input = val_input.cuda()
            val_label = val_label.cuda()
        score = model(val_input)
        confusion_matrix.add(score.data.squeeze(), label.type(t.LongTensor))

    model.train()
    cm_value = confusion_matrix.value()
    accuracy = 100. * (cm_value[0][0] + cm_value[1][1]) / (cm_value.sum())
    return confusion_matrix, accuracy


if __name__ == '__main__':
    fire.Fire()