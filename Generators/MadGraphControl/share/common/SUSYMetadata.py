
from MadGraphControl.MadGraphUtils import build_param_card as build_slha_file

# Retrieve sparticle masses from a slha file

def find_in_slha_file(slha_file, blockname, find):
  """
  slha_file: file name for spectrum file in SLHA format including decay information
  blockname: name of block to work on
  find: 3-tuple - 1: index for compare tag, 2 - tag to compare to, 3 - index of return value
  """

  search = False
  parameter = None

  for l in file(slha_file).readlines():
    if l.upper().startswith('BLOCK %s' % blockname.upper()):
      search = True
    elif l.upper().startswith('BLOCK'):
      search = False
      
    if not search:
      continue

    if l.split()[find[0]] == find[1]:
      parameter = l.split()[find[2]]
        
  return parameter

def mass_extract(slha_file, pids):

  masses = []

  # extract
  # (find block: Block MASS, pid in first column (index 0), mass in second (index 1))
  for id in pids:
    masses += [find_in_slha_file(slha_file, 'MASS', [0, id, 1])]

  return masses

def remove_decay(slha_file, pid):
  """
  rewrite slha file, removing DECAY block for given pid and remove decay from MSSM.model file
  """

  # rewrite slha file
  import shutil
  shutil.move(slha_file, slha_file+'.org')
  
  update = open(slha_file, 'w')

  skip = False
  for l in open(slha_file+'.org'):
    if l.startswith('DECAY   %s '%pid):
      skip = True
      continue

    if l.startswith('DECAY '):
      skip = False
    
    if not skip:
      update.write(l)
    
  update.close()

  # remove also from MSSM.model file

  # rewrite MSSM.model file
  from PyJobTransformsCore.trfutil import get_files
  get_files( 'MSSM.model', keepDir=False, errorIfNotFound=True )

  import shutil
  shutil.move('MSSM.model', 'MSSM.model.org')
  
  update = open('MSSM.model', 'w')

  for l in open('MSSM.model.org'):
    if l.startswith('insert /Herwig/Shower/ShowerHandler:DecayInShower 0  %s '%pid):
      continue
    update.write(l)
    
  update.close()
