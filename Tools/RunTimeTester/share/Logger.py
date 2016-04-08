class Logger:
    def __init__(self):
        self.debugL    = []
        self.infoL     = []
        self.warningL  = []
        self.errorL    = []
        self.criticalL = []

    def debug(self, line):    self.debugL.append(line)
    def info(self, line):     self.infoL.append(line)
    def warning(self, line):  self.debugL.append(line)
    def error(self, line):    self.errorL.append(line)
    def critical(self, line): self.criticalL.append(line)
