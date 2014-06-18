# This is a new patmerge template for TrigFTKSim/standalone/patmerge executable.
# It can merge an unlimited number of banks, and can read/output bzip'ed files
# Below is a sample template: (or use ftk.py to generate one for you)

#NINPUTS 4
#/scratch/antonk/pattgen_raw_4L_20x156I225x25_reg1.patt.bz2
#/scratch/antonk/pattgen_raw_4L_20x156I225x25_reg2.patt.bz2
#/scratch/antonk/pattgen_raw_4L_20x156I225x25_reg3.patt.bz2
#/scratch/antonk/pattgen_raw_4L_20x156I225x25_reg4.patt.bz2
#OUT_FILE /scratch/antonk/out_reg1.patt.bz2
#VMEM_LIMIT 2500000
#DELETE_FILES_AFTER_USE   0
