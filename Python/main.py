import fire
from config import cfg
def train (**kwargs):
    cfg.__parse__(kwargs)







if __name__ == '__main__':
    fire.Fire()