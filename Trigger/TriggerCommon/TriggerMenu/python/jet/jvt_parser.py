import re
jvt_re = re.compile(r'((?P<njet>[1-9]))?jvt(?P<workingpoint>\d{3})?\et(?P<etmin>\d{2,3})$')

def _jvt_parse(s):
  """Parse the incoming string to find the working point, number of jets and minimum et"""

  result = {}
  m = jvt_re.match(s)
  if m is None: return(True, {}) # error
  d = m.groupdict()

  key_map = {
    'njet':  ('njet','n'),
    'wp':    ('workingpoint','value'),
    'etmin': ('etmin','min'),
  }

  for k, v in key_map.items():
    if d[v[0]] is not None: result[k] = {v[1]: d[v[0]]}

  return (False, result)

def _jvt_decode(s):
  result = {
    'njet':  {'n':'1'},
    'wp':    {'value':'011'}, # other options are medium and tight
    'etmin': {'min':'15'},
  }
  error, d = _jvt_parse(s)
  if error: return(error, {})

  for k in d: result[k] = d[k]

  return (False, result)

def jvt_parser(s, args):
  assert 'jvt' in s
  err, jvt_params = _jvt_decode(s)
  if err: return True # error

  args['njet']  = jvt_params['njet']['n']
  args['wp']    = jvt_params['wp']['value'][1:]
  args['etmin'] = jvt_params['etmin']['min']

  return False

# For testing purposes
if __name__ == '__main__':
  examples = [
    'jvt1',
    'jvt1et15',
    'jvt11',
    'jvt11et15',
    'jvt011',
    '4jvt011',
    'jvt011et',
    '4jvt011et',
    'jvt011et15',
    '4jvt011et15',
  ]
  
  # Testing jvt_parser
  for s in examples:
    args = {}	  
    err = jvt_parser(s,args)
    if err: print 'ERROR, '+s+' chain not supported'
    else:
      print s+":"
      for k in args.keys(): print k, args[k]
