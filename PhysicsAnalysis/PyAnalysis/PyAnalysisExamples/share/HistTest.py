######################################################################
#
# This script demonstrates how to use commands related to histogram
#
#  usage:
#   * book
#   h1 = book (path, title, nbinx, xmin, ymin)
#   h2 = book (path, title, nbinx, xmin, ymin, nbiny, ymin, ymax)
#   hp = bookProf (path, title, nbinx, xmin, ymin)
#
#   * fill
#   fill (hist, Class#Key, parameter, criteria=True, nEvent=100)
#   fill2 (hist, Class#Key, paramX, paramY, criteria=True, nEvent=100)
#   fillProf (hist, Class#Key, parameter, criteria=True, nEvent=100)
#
#   * draw
#   h.Draw()
#
#####################################################################

theApp.initialize()

# book 1D hist
h1 = book ('/stat/test1', 'Test1', 100, 0, 100*GeV)
# book 2D hist
h2 = book ('/stat/test2', 'Test2', 100, 0, 100*GeV, 100, -50*GeV, 50*GeV)
# book profile hist
hp = bookProf ('/stat/testp', 'Testp', 100, 0, 100*GeV)

# fill. For parameters, see PlotTest.py
fill (h1, "ElectronContainer#ElectronCollection","$x.pt()", "$x.pz()<10*GeV")
fill2 (h2, "ElectronContainer#ElectronCollection","$x.pt()", "$x.px()", "$x.pz()<10*GeV")
fillProf (hp, "ElectronContainer#ElectronCollection","$x.pt()", "$x.px()", "$x.pz()<10*GeV")

# draw
h1.Draw()
h2.Draw()
hp.Draw()

# all ROOT functions are available
h1.Fit('gaus')
h2.ProjectionX().Draw()

# Note that the fill commands can be used for ROOT histograms as well.
import ROOT
h = ROOT.TH1F('rtest','Rtest',100,0,100*GeV)
fill (h, "ElectronContainer#ElectronCollection","$x.pt()", "$x.pz()<10*GeV")
h.Draw()
