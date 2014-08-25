include.block( "MissingETPerformance/Linearity_jobOptions.py" )

from MissingETPerformance.MissingETPerformanceConf import \
     LinearityTool as ConfiguredLinearityTool
LinearityTool = ConfiguredLinearityTool(

  lnumptt        = 20,
  lstartpt       = 0, 
  lbinpt         = 5.,
  fitrangel      = 2,  
  lnbini         = 50,    
  percerr        = 0.15, 
  entries        = 50,
  curvaslin      = 0.3, 
  fitr           = 2.5,
  linplot_x = 0.72,
  linplot_y = 0.85,
  linplot_text = 'W#rightarrow e#nu',
  linorm       = 0,
  bin_rp3    = 1.,
  suptmi=100.,
  etmi_threshold=50.,
    )

ToolSvc += LinearityTool
print      LinearityTool
