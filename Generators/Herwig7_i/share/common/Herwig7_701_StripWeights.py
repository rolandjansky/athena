import os, shutil

m_inFile = runArgs.inputGeneratorFile

if not m_inFile:
    exit()

# Pick name Prefix
# - if original file is compressed then athena places the uncompressed
#   LHE file in the current working directory before this script is executed
m_Prefix = None
if m_inFile is not None:
    if '.tar.gz' in m_inFile:
        m_Prefix = os.path.split(m_inFile)[1].split('.tar.gz')[0]
    elif '.events' in m_inFile:
        m_Prefix = os.path.split(m_inFile)[1].split('.events')[0]
        abs_path = os.path.split(os.path.abspath(m_inFile))[0]
        cwd_path = os.getcwd()
        if abs_path != cwd_path: # if original file is in a different folder
            # uncompressed LHE file in a different folder is not supported by job transform,
            # rectify this by copying the original LHE file to the current working directory
            shutil.copyfile(m_inFile, os.path.split(m_inFile)[1])

m_lhe_extracted_old = m_Prefix+".events"
m_lhe_extracted_new = "new."+m_Prefix+".events"

if m_Prefix is None:
    raise RuntimeError("File title could not be determined because of unknown file extension")

# Strip Weights from old LHE file and write new LHE file
weights = False
with open(m_lhe_extracted_old, "r") as lhef_in:
    with open(m_lhe_extracted_new, "w") as lhef_out:
        
        for line in lhef_in:
        
            # find tags related to multiple/optional weights
            if "<initrwgt>" in line or "<rwgt>" in line:
                weights = True
                
            # propagate line if we are currently not in a line
            # related to the optional/multiple event weights
            if not weights:
                lhef_out.write(line)

            # remember end of optional/multiple event weights lines
            if "</initrwgt>" in line or "</rwgt>" in line:
                weights = False

# Delete old (unstripped) LHE file
os.remove(m_lhe_extracted_old)

# Rename new (stripped) LHE file to the old filename
shutil.move(m_lhe_extracted_new, m_lhe_extracted_old)
