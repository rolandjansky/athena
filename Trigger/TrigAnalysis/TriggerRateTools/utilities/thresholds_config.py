# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Tkinter import *
import tkMessageBox
import tkFileDialog
from decimal import *
import copy
import sys


class App:

    def __init__(self, master):
        self.master = master
        self.frame = Frame(master, borderwidth=10)
        self.frame.grid(row=0, column=0, sticky=NSEW)

        self.modified = FALSE
        self.sigfile = ""

        # Create output lists
        self.outputUndo = []
        self.outputRedo = []
        self.output = []

        # Create and arrange widgets

        self.thrFillList=[]
        self.createWidgets(self.frame)


        # Bind Events to Widgets
        #self.bindWidgets()

    def createWidgets(self, frame):
        total_width=5

        self.thrFillFrame = Frame(frame, relief=GROOVE, borderwidth=2)
        self.outputFrame = Frame(frame, relief=GROOVE, borderwidth=2)

        # Create a Label to display a header
        self.headLbl = Label(frame, text="Thresholds Configuration Utility", relief=RIDGE)
        self.headLbl.grid(row=0, column=0, columnspan=total_width, sticky=NSEW)


        self.lblSpacer1 = Label(frame)
        self.lblSpacer1.grid(row=1, column=0, columnspan=total_width, sticky=NSEW)

        self.lblSigName = Label(frame, text='Trigger Element:', anchor=W)
        self.lblAlgo    = Label(frame, text='Algorithm:', anchor=W)
        self.lblParam   = Label(frame, text='Parameter:', anchor=W)
        self.lblThr     = Label(frame, text='Threshold:', anchor=W)
        
        self.valSigName = StringVar(self.master)
        self.valSigName.set("")
        self.valAlgo = StringVar(self.master)
        self.valAlgo.set("")
        self.valParam = StringVar(self.master)
        self.valParam.set("")
        self.valThr = StringVar(self.master)
        self.valThr.set("")
        
        self.entSigName = Entry(frame, bg="white", textvariable=self.valSigName, width=30)
        self.entAlgo    = Entry(frame, bg="white", textvariable=self.valAlgo, width=30)
        self.entParam   = Entry(frame, bg="white", textvariable=self.valParam, width=30)
        self.entThr     = Entry(frame, bg="white", textvariable=self.valThr, width=30)

        self.btnSelectSigName = Button(frame, text='List', command=self.getSig, state=DISABLED)
        self.btnGetSigFile = Button(frame, text='Get Signatures', command=self.getSigFile, state=NORMAL)
        self.btnAdd = Button(frame, text='Add', command=self.updateThreshold, state=NORMAL)

        self.btnUndo = Button(frame, text='Undo', command=self.undo, state=DISABLED)
        self.btnRedo = Button(frame, text='Redo', command=self.redo, state=DISABLED)
        self.btnOpen = Button(frame, text='Open', command=self.open, state=NORMAL)
        self.btnSaveAs = Button(frame, text='Save As', command=self.saveAs, state=NORMAL)
        self.btnQuit = Button(frame, text='Exit', command=self.quit, state=NORMAL)
        self.btnHelp = Button(frame, text='Help', command=self.help, state=NORMAL)

        self.btnRefresh = Button(self.outputFrame, text='Refresh', command=self.refreshOutput, state=NORMAL)
        self.btnDelete  = Button(self.outputFrame, text='Delete Selected', command=self.deleteSel, state=NORMAL)
        self.btnClear   = Button(self.outputFrame, text='Delete All ', command=self.clearOutput, state=NORMAL)

        # Create the Expert Checkbutton
        self.valThrFill = BooleanVar(self.master)
        self.valThrFill.set(FALSE)
        self.thrFillCheckBtn = Checkbutton(frame, text="Expert Mode", command=self.updateThrFillStatus,
                    variable=self.valThrFill)

        # Create Filling Option RadioButton
        self.radioBtnsFill = []
        self.radioValFill = StringVar(self.master)
        btnListFill = ("Constant", "Range")
        for btn in btnListFill:
            self.radioBtnsFill.append(Radiobutton(self.thrFillFrame, text=btn, value=btn, indicatoron=TRUE,
                                               variable=self.radioValFill))
        self.radioValFill.set("Constant")

        for btn in self.radioBtnsFill:
            self.thrFillList.append(btn)

        # Create SampleOutput List
        self.lblOutput = Label(frame, text='Output Display:', anchor=W)
        self.outputScrollY = Scrollbar(self.outputFrame, orient=VERTICAL)
        self.outputScrollX = Scrollbar(self.outputFrame, orient=HORIZONTAL)
        self.outputListbox = Listbox(self.outputFrame, yscrollcommand=self.outputScrollY.set, 
                                    xscrollcommand=self.outputScrollX.set, 
                                    bg="white", selectmode=EXTENDED, width=55)
        self.outputScrollY.config(command=self.outputListbox.yview)
        self.outputScrollX.config(command=self.outputListbox.xview)

        self.lblThrRepeat = Label(self.thrFillFrame, text='Repeat #:', anchor=W)
        self.entThrRepeat = Entry(self.thrFillFrame, bg="white", width=10)
        
        self.lblThrPrepend = Label(self.thrFillFrame, text='Prepend text:', anchor=W)
        self.lblThrMin     = Label(self.thrFillFrame, text='Initial Threshold:', anchor=W)
        self.lblThrNum     = Label(self.thrFillFrame, text='# of iterations:', anchor=W)
        self.lblThrInc     = Label(self.thrFillFrame, text='Increment / iter.:', anchor=W)
        self.lblThrAppend  = Label(self.thrFillFrame, text='Append text:', anchor=W)

        self.entThrPrepend = Entry(self.thrFillFrame, bg="white", width=30)
        self.entThrMin     = Entry(self.thrFillFrame, bg="white", width=30)
        self.entThrNum     = Entry(self.thrFillFrame, bg="white", width=30)
        self.entThrInc     = Entry(self.thrFillFrame, bg="white", width=30)
        self.entThrAppend  = Entry(self.thrFillFrame, bg="white", width=30)

        self.thrFillList.append(self.lblThrRepeat)
        self.thrFillList.append(self.entThrRepeat)
        self.thrFillList.append(self.lblThrPrepend)
        self.thrFillList.append(self.lblThrMin)
        self.thrFillList.append(self.lblThrNum)
        self.thrFillList.append(self.lblThrInc)
        self.thrFillList.append(self.lblThrAppend)
        self.thrFillList.append(self.entThrPrepend)
        self.thrFillList.append(self.entThrMin)
        self.thrFillList.append(self.entThrNum)
        self.thrFillList.append(self.entThrInc)
        self.thrFillList.append(self.entThrAppend)

        # Use the Grid Geomatry manager to arrange the widgets
        self.lblSigName.grid(row=2, column=0, columnspan=1, sticky=W)
        self.lblAlgo.grid(row=3, column=0, columnspan=1, sticky=W)
        self.lblParam.grid(row=4, column=0, columnspan=1, sticky=W)
        self.lblThr.grid(row=5, column=0, columnspan=1, sticky=W)

        self.entSigName.grid(row=2,column=1,columnspan=1, sticky=W)
        self.entAlgo.grid(row=3,column=1,columnspan=1, sticky=W)
        self.entParam.grid(row=4,column=1,columnspan=1, sticky=W)
        self.entThr.grid(row=5,column=1,columnspan=1, sticky=W)

        self.btnSelectSigName.grid(row=2, column=2, columnspan=1, sticky=NSEW)
        self.btnGetSigFile.grid(row=2, column=3, columnspan=2, sticky=NSEW)
        self.btnAdd.grid(row=4, column=2, columnspan=1, rowspan=2, sticky=NSEW)

        self.btnUndo.grid(row=4, column=3, rowspan=2, columnspan=1, sticky=NSEW)
        self.btnRedo.grid(row=4, column=4, rowspan=2, columnspan=1, sticky=NSEW)
        
        self.lblSpacer3 = Label(frame)
        self.lblSpacer3.grid(row=8, column=3, rowspan=8, columnspan=1, sticky=NSEW)
        self.btnOpen.grid(row=8, column=4, rowspan=1, columnspan=2, sticky=NSEW)
        self.btnSaveAs.grid(row=9, column=4, rowspan=1, columnspan=2, sticky=NSEW)
        self.btnQuit.grid(row=10, column=4, rowspan=1, columnspan=2, sticky=NSEW)
        self.btnHelp.grid(row=11, column=4, rowspan=1, columnspan=2, sticky=NSEW)

        self.lblSpacer2 = Label(frame)
        self.lblSpacer2.grid(row=6, column=0, columnspan=total_width, sticky=NSEW)

        self.thrFillCheckBtn.grid(row=7, column=0, columnspan=1, sticky=NSEW)

        self.thrFillFrame.grid(row=8, column=0, columnspan=3, rowspan=8, sticky=NSEW)
        self.radioBtnsFill[0].grid(row=0, column=0, columnspan=1, sticky=W)
        self.lblThrRepeat.grid(row=1, column=1, columnspan=1, sticky=W)
        self.entThrRepeat.grid(row=1, column=2, columnspan=1, sticky=W)
        
        self.radioBtnsFill[1].grid(row=2, column=0, columnspan=1, sticky=W)
        self.lblThrPrepend.grid(row=3, column=1, columnspan=1, sticky=W)
        self.lblThrMin.grid(row=4, column=1, columnspan=1, sticky=W)
        self.lblThrNum.grid(row=5, column=1, columnspan=1, sticky=W)
        self.lblThrInc.grid(row=6, column=1, columnspan=1, sticky=W)
        self.lblThrAppend.grid(row=7, column=1, columnspan=1, sticky=W)
                          
        self.entThrPrepend.grid(row=3, column=2, columnspan=1, sticky=W)
        self.entThrMin.grid(row=4, column=2, columnspan=1, sticky=W)
        self.entThrNum.grid(row=5, column=2, columnspan=1, sticky=W)
        self.entThrInc.grid(row=6, column=2, columnspan=1, sticky=W)
        self.entThrAppend.grid(row=7, column=2, columnspan=1, sticky=W)

        self.lblOutput.grid(row=16, column=0, columnspan=1, rowspan=1, sticky=NSEW)
        self.outputFrame.grid(row=17, column=0, columnspan=6, rowspan=8, sticky=NSEW)
        self.outputListbox.grid(row=0, column=0, columnspan=1, rowspan=5, sticky=NSEW)
        self.outputScrollY.grid(row=0, column=1, columnspan=1, rowspan=5, sticky=NSEW)
        self.outputScrollX.grid(row=6, column=0, columnspan=1, rowspan=1, sticky=NSEW)
        self.lblSpacer4 = Label(self.outputFrame)
        self.lblSpacer4.grid(row=0, column=2, rowspan=8, columnspan=1, sticky=NSEW)
        self.btnRefresh.grid(row=0, column=3, rowspan=1, columnspan=2, sticky=NSEW)
        self.btnDelete.grid(row=1, column=3, rowspan=1, columnspan=2, sticky=NSEW)
        self.btnClear.grid(row=2, column=3, rowspan=1, columnspan=2, sticky=NSEW)

        
        #self.btnSelectSigName.configure(state=DISABLED)
        self.updateThrFillStatus()

        #self.outputListbox.insert(0,"test0")
        #self.outputListbox.insert(1,"test1")
        #self.outputListbox.insert(2,"test2")

    def updateThrFillStatus(self):
        status  = self.valThrFill.get()
        objList = self.thrFillList
        if status:
            for obj in objList:
                obj.configure(state=NORMAL)
        else:
            for obj in objList:
                obj.configure(state=DISABLED)


    def updateThreshold(self):
        sigName=self.entSigName.get()
        algo=self.entAlgo.get()
        param=self.entParam.get()
        thr=self.entThr.get()

        sigName=sigName.expandtabs(1)
        algo=algo.expandtabs(1)
        param=param.expandtabs(1)
        thr=thr.expandtabs(1)

        if sigName.count(" ") > 0:
            tkMessageBox.showwarning("Warning", "Please remove any spaces / tabs from the TRIGGER ELEMENT name and retry.")
            return -1

        if algo.count(" ") > 0:
            tkMessageBox.showwarning("Warning", "Please remove any spaces / tabs from the ALGORITHM name and retry.")
            return -1

        if param.count(" ") > 0:
            tkMessageBox.showwarning("Warning", "Please remove any spaces / tabs from the PARAMETER name and retry.")
            return -1

        if thr.count(" ") > 0:
            tkMessageBox.showwarning("Warning", "Please remove any spaces / tabs from the THREHOLD value and retry.")
            return -1

        if sigName == "":
            tkMessageBox.showwarning("Warning", "Empty TRIGGER ELEMENT name.")
            return -1

        if algo == "":
            tkMessageBox.showwarning("Warning", "Empty ALGORITHM name.")
            return -1

        if param == "":
            tkMessageBox.showwarning("Warning", "Empty PARAMETER name.")
            return -1
        
        if self.valThrFill.get():
            if self.radioValFill.get() == "Constant":
                if thr == "":
                    tkMessageBox.showwarning("Warning", "Empty THRESHOLD value.")
                    return -1
                
                num_thr=0

                try:
                    num_thr=int(self.entThrRepeat.get())

                except:
                    tkMessageBox.showwarning("Warning", "Invalid \"repeat #\".")
                    return -1

                self.outputUndo.append(copy.deepcopy(self.output))
                self.outputRedo=[]
                self.modified = TRUE
                for index in range(num_thr):
                    self.addThreshold(sigName, algo, param, thr)

            elif self.radioValFill.get() == "Range":

                curr_prepend=self.entThrPrepend.get()
                curr_append=self.entThrAppend.get()

                curr_prepend=curr_prepend.expandtabs(1)
                curr_append=curr_append.expandtabs(1)

                if curr_prepend.count(" ") > 0:
                    tkMessageBox.showwarning("Warning", "Please remove any spaces / tabs from the PREPEND text and retry.")
                    return -1

                if curr_append.count(" ") > 0:
                    tkMessageBox.showwarning("Warning", "Please remove any spaces / tabs from the APPEND text and retry.")
                    return -1

                
                curr_thr=Decimal("0.0")
                curr_inc=Decimal("1.0")
                curr_num=Decimal("0.0")
                max_num=Decimal("0.0")

                try:
                    curr_thr=Decimal(self.entThrMin.get())

                except:
                    tkMessageBox.showwarning("Warning", "Initial threshold cannot be converted to a Decimal.")
                    return -1

                try:
                    max_num=Decimal(self.entThrNum.get())+Decimal("1.0")

                except:
                    tkMessageBox.showwarning("Warning", "Number of thresholds cannot be converted to a Decimal.")
                    return -1

                try:
                    curr_inc=Decimal(self.entThrInc.get())

                except:
                    tkMessageBox.showwarning("Warning", "Increment value cannot be converted to a Decimal.")
                    return -1

                self.outputUndo.append(copy.deepcopy(self.output))
                self.outputRedo=[]
                self.modified = TRUE
                while curr_num < max_num:
                    self.addThreshold(sigName, algo, param, curr_prepend+str(curr_thr+(curr_num*curr_inc))+curr_append)
                    curr_num=curr_num+Decimal("1.0")


        else:
            if thr == "":
                tkMessageBox.showwarning("Warning", "Empty THRESHOLD value.")
                return -1

            self.outputUndo.append(copy.deepcopy(self.output))
            self.outputRedo=[]
            self.modified = TRUE
            self.addThreshold(sigName, algo, param, thr)

        return 0

    def addThreshold(self, sigName, algo, param, thr):
        
        not_found=TRUE

        for line in self.output:
            if not_found:
                if sigName == line[0]:
                    if algo == line[1]:
                        if param == line[2]:
                            line.append(thr)
                            not_found=FALSE
        
        if not_found:
            new_line=[sigName, algo, param, thr]
            self.output.append(new_line)

        self.refreshOutput()

    def refreshOutput(self):
        self.outputListbox.delete(0, END)
        
        for line in self.output:
            str=""
            for token in line:
                str=str+token+'  '

            self.outputListbox.insert(END,str)

        self.updateStatusBtns()

    def clearOutput(self):
        if len(self.output) > 0:
            if tkMessageBox.askyesno("Clear Data", "Are you sure you want to delete all the thresholds?"):
                self.outputUndo.append(copy.deepcopy(self.output))
                self.outputRedo=[]
                self.modified = TRUE
                self.output=[]
                self.refreshOutput()
    
    def deleteSel(self):
        delete_list = self.outputListbox.curselection()
        if len(delete_list) > 0:
            self.outputUndo.append(copy.deepcopy(self.output))
            self.outputRedo=[]
            self.modified = TRUE

            count = 0
            for token in delete_list:
                del self.output[int(token)-count]
                count = count+1
            
            self.refreshOutput()
    
    def open(self):
        input_lines = []
        parsed_lines = []
        input_file=tkFileDialog.askopenfile(mode='r',initialfile='thresholds.dat',filetypes=[("Data file","*.dat"),("All Files","*")])

        if input_file != None:
            input_lines=input_file.readlines()
            input_file.close()

            for line in input_lines:
                parsed = self.SplitLines(line)
                if len(parsed) > 3:
                    parsed_lines.append(parsed)

            question = TRUE
            if len(self.output) > 0:
                if self.modified:
                    question = tkMessageBox.askyesno("Append?", "Would you like to append the file to the current output? (no to replace it)")
                    if not question:
                        self.clearOutput()

            for line in parsed_lines:
                thr_index = 3
                while thr_index < len(line):
                    self.valSigName.set(line[0])
                    self.valAlgo.set(line[1])
                    self.valParam.set(line[2])
                    self.valThr.set(line[thr_index])
                    self.updateThreshold()
                    thr_index = thr_index+1

            self.valSigName.set("")
            self.valAlgo.set("")
            self.valParam.set("")
            self.valThr.set("")
            
            self.refreshOutput()

        else:
            tkMessageBox.showwarning("Warning", "Could not open the file, please try again")
        
    def saveAs(self):
        output_file=tkFileDialog.asksaveasfilename(initialfile='thresholds.dat',filetypes=[("Data file","*.dat"),("All Files","*")])

        if not output_file == "":
            try:
                fw = open(output_file,'w')
                for line in self.output:
                    str=line[0]
                    count = 0
                    for token in line:
                        if count > 0:
                            str=str+'\t'+token
                        count = count+1
       
                    fw.write(str+'\n')

                self.modified = FALSE
            except:
                tkMessageBox.showwarning("Warning", "Could not write out to specified file, please try again")
            
    def undo(self):
        self.modified = TRUE

        self.outputRedo.append(copy.deepcopy(self.output))

        if len(self.outputUndo) > 0:
            self.output=self.outputUndo.pop()
        else:
            self.output=[]

        self.refreshOutput()

    def redo(self):
        self.modified = TRUE

        self.outputUndo.append(copy.deepcopy(self.output))

        if len(self.outputRedo) > 0:
            self.output=self.outputRedo.pop()

        self.refreshOutput()

    def updateStatusBtns(self):
        if len(self.outputUndo) > 0:
            self.btnUndo.configure(state=NORMAL)
        else:
            if len(self.output) > 0:
                self.btnUndo.configure(state=NORMAL)
            else:
                self.btnUndo.configure(state=DISABLED)

        if len(self.outputRedo) > 0:
            self.btnRedo.configure(state=NORMAL)
        else:
            self.btnRedo.configure(state=DISABLED)


    def quit(self):
        sure = TRUE
        if len(self.output) > 0:
            if self.modified:
                sure = tkMessageBox.askyesno("Unsaved data", "The modifications since your last save will be lost. Continue?")
        
        if sure:
            sys.exit()

    def help(self):
        
        self.helpPopup=Toplevel()
        self.helpPopup.title("Help")
        helpMainFrame = Frame(self.helpPopup, borderwidth=10)
        helpMainFrame.grid(row=0, column=0, sticky=NSEW)

        helpHeadLbl = Label(helpMainFrame, text="Help", relief=RIDGE)
        helpHeadLbl.grid(row=0, column=0, columnspan=6, sticky=NSEW)

        helpLblSpacer1 = Label(helpMainFrame)
        helpLblSpacer2 = Label(helpMainFrame)
        helpLblSpacer1.grid(row=1, column=0, columnspan=6, sticky=NSEW)

        helpBtnQuit = Button(helpMainFrame, text='Exit', command=self.helpPopup.destroy, state=NORMAL)

        helpScrollY = Scrollbar(helpMainFrame, orient=VERTICAL)
        helpText = Text(helpMainFrame, yscrollcommand=helpScrollY.set, wrap=WORD, state=NORMAL,
                                    bg="white")
        helpScrollY.config(command=helpText.yview)

        helpText.grid(row=2, column=0, columnspan=5, rowspan=4, sticky=NSEW)
        helpScrollY.grid(row=2, column=5, columnspan=1, rowspan=4, sticky=NSEW)

        helpLblSpacer2.grid(row=6, column=0, columnspan=6, sticky=NSEW)
        helpBtnQuit.grid(row=7, column=4, columnspan=2, sticky=NSEW)

        helpText.tag_config('bold_text', font=('times', 12, 'bold'))
        helpText.tag_config('section', font=('times', 14, 'bold', 'underline'))

        
        helpText.insert(END, 'Basic Mode', 'section')
        helpText.insert(END, '\n\nTrigger Element:', 'bold_text')
        helpText.insert(END, '\nThis should be the name of the trigger element you wish to change the threshold for.'
                                +' This is tipically the last trigger element of the given signature (e.g. \"EF_jet20aEt\").')
        helpText.insert(END, '\n\nAlgorithm:', 'bold_text')
        helpText.insert(END, '\nThis should be the name of the algorithm\'s instance name (as defined in the hypothesis jobOptions)'
                                +' you wish to change the threshold for (e.g. \"TrigEFConeJetHypo_i13a_EF\").')
        helpText.insert(END, '\n\nParameter:', 'bold_text')
        helpText.insert(END, '\nThis should be the name of the parameter (as defined in the hypothesis jobOptions) whose cut value'
                                +' you wish to change (e.g. \"Etcut\" would be used in the jobOptions as'
                                +' \"TrigEFConeJetHypo_i13a_EF.Etcut=<threshold>\").')
        helpText.insert(END, '\n\nThreshold:', 'bold_text')
        helpText.insert(END, '\nThis should be the value of the new threshold for the specific parameter and algorithm'
                                +' (e.g. \"20*GeV\" would be used in the jobOptions as'
                                +' \"TrigEFConeJetHypo_i13a_EF.Etcut=20*GeV\").')
        helpText.insert(END, '\n\n\nUsage:', 'bold_text')
        helpText.insert(END, '\nIn basic mode, the user should enter a value for the trigger element, algorithm, parameter and threshold.'
                                +' The user can retrieve the list of available trigger elements from the \'signatures.dat\' configuration'
                                +' file by first using the \"Get Signatures\" button, then using the \"List\" button from which he can'
                                +' select a trigger element. Once this information has been input, clicking on the \"Add\" button will'
                                +' add the threshold to the configuration, and display it in the \"Output Display\" window.')
        helpText.insert(END, '\n\n\n', 'bold_text')
        helpText.insert(END, 'Expert Mode', 'section')
        helpText.insert(END, '\n\nConstant:', 'bold_text')
        helpText.insert(END, '\nThe Constant mode allows the user to add the same threshold several times, For instance, if the threshold'
                                +'should be constant for the first 10 sets of thresholds, then the user can enter the value "10" in the'
                                +'"Repeat #" field.')
        helpText.insert(END, '\n\nRange:', 'bold_text')
        helpText.insert(END, '\nThe Range mode allows the user to iteratively create thresholds, This mode requires an initial threshold,'
                                +'as well as a number of subsequent interations (using "3" will result in 4 thresholds being created), and'
                                +'finally an increment value per iteration. The tool can only iterate over floats, but the "Prepend text"'
                                +'and the "Append text" allow the user to insert any text before and after the value being iterated over.')


        helpText.configure(state=DISABLED)

        self.helpPopup.mainloop()

    def getSigFile(self):
        self.sigfile=tkFileDialog.askopenfilename(initialfile='signatures.dat',filetypes=[("Data file","*.dat"),("All Files","*")])

        if self.sigfile == "":
            self.btnSelectSigName.configure(state=DISABLED)
        else:
            self.btnSelectSigName.configure(state=NORMAL)

    def getSig(self):
        
        sigs = []

        try:
            f_sigs = open(self.sigfile,'r')
            lines = f_sigs.readlines()
            for line in lines:
                tup = self.SplitLines(line)
                if len(tup) == 4:
                    if (tup[0] == 'L1') or (tup[0] == 'L2') or (tup[0] == 'EF'):
                        sigs.append(tup[3])

        except:
            tkMessageBox.showwarning("Warning", "Could not parse the given signature file.")

        self.sigPopup=Toplevel()
        self.sigPopup.title(self.sigfile)
        sigMainFrame = Frame(self.sigPopup, borderwidth=10)
        sigMainFrame.grid(row=0, column=0, sticky=NSEW)

        sigHeadLbl = Label(sigMainFrame, text="Select Signature", relief=RIDGE)
        sigHeadLbl.grid(row=0, column=0, columnspan=3, sticky=NSEW)

        sigLblSpacer1 = Label(sigMainFrame)
        sigLblSpacer1.grid(row=1, column=0, columnspan=3, sticky=NSEW)

        sigBtnSelect = Button(sigMainFrame, text='Select', command=self.selectSig, state=NORMAL)
        sigBtnQuit = Button(sigMainFrame, text='Exit', command=self.sigPopup.destroy, state=NORMAL)

        sigScrollY = Scrollbar(sigMainFrame, orient=VERTICAL)
        sigScrollX = Scrollbar(sigMainFrame, orient=HORIZONTAL)
        self.sigListbox = Listbox(sigMainFrame, yscrollcommand=sigScrollY.set, 
                                    xscrollcommand=sigScrollX.set, 
                                    bg="white", width=50)
        sigScrollY.config(command=self.sigListbox.yview)
        sigScrollX.config(command=self.sigListbox.xview)

        self.sigListbox.grid(row=2, column=0, columnspan=1, rowspan=4, sticky=NSEW)
        sigScrollY.grid(row=2, column=1, columnspan=1, rowspan=4, sticky=NSEW)
        sigScrollX.grid(row=3, column=0, columnspan=1, rowspan=4, sticky=NSEW)

        sigBtnSelect.grid(row=3, column=2, columnspan=1, sticky=NSEW)
        sigBtnQuit.grid(row=2, column=2, columnspan=1, sticky=NSEW)

        for sig in sigs:
            self.sigListbox.insert(END,sig)

        self.sigPopup.mainloop()

    def selectSig(self):
        self.entSigName.delete(0, END)
        self.entSigName.insert(END, self.sigListbox.get(ACTIVE))
        self.sigPopup.destroy()

    def SplitLines (self,line):
        line = line.strip()
        new_list = line.split()
    
        return new_list


if __name__ == '__main__':
    root = Tk()
    app = App(root)
    app.master.title("TriggerRateTools Configuration Utility")
    root.mainloop()
