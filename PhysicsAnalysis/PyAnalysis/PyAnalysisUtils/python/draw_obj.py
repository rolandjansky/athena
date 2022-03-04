# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#
# File: draw_obj.py
# Created: sss, 2004.
# Purpose: Helpers for drawing ROOT objects.
#

"""Helpers for drawing ROOT objects.
The main function in here is draw_obj(obj [, options]).
This is similar to obj->Draw(options), but some extra
functionality is added:

  - Automatic zone handling.  You can split the canvas into
    subpads (zones) with zone().  Each call to draw_obj() will
    plot into the next sequential pad (unless SAME was specified).
    Selecting a pad by clicking with button 2 will change
    the sequence so that the next objects is drawn in the
    selected pad.  draw_obj also takes an optional padnum
    argument to explicitly specify the pad number to use.

  - If the SAME option is given, then the new object will be
    superimposed on an existing one in the current pad.
    In the case of 1D histograms, we change the line
    type of the new histogram, and also rescale the vertical
    axis, if needed to make the new histogram fit.

  - The option MERGE is like SAME, except that we step through
    the pads as normal.  The new plot will be made on top of
    whatever happens to be on that pad.  By default, the line
    type used will be the same as one would get the first time
    one overlays a plot with SAME; to override this, put a number
    directly after the option, like `MERGE2'.

  - The option NORM draws the histogram normalized to 1.

  - The option LOGY draws the histogram using a logarithmic
    y-axis.

  - The line type will recycle after four histograms have
    been superimposed.  If the LINECOLORS option is given, then
    the new histograms will be drawn in a different color.

  - draw_obj takes optional min and max parameters to set the
    minimum and maximum values for the y-axis of the histogram
    being drawn.

Besides draw_obj, a few other functions are available.
zone(nx,ny) was already mentioned.
printeps(fname) is a shortcut to print an eps file from the current canvas.
get_canvas() returns the canvas currently being used for drawing.
get_pad() returns the pad currently being used for drawing.
"""
from __future__ import division


from ROOT import gROOT, TCanvas, TVirtualPad, TH1, TH2, TObject


# The number of histograms we've superimposed on the current pad with SAME.
_samecount = 0

# The number of the last subpad we drew in.
# Will be 0 if there is only one pad.
_lastpad = 0

# The number of the next subpad to draw in, starting with 1.
_nextpad = 1

# Total number of subpads.
_npads = 1



class _options(object):
    """Helper class for parsing options."""
    def __init__ (self, options):
        self.merge = 0
        self.same = 0
        self.norm = 0
        self.logy = 0
        self.fill = -1 
        self.linetype = -1
        self.color = -1
        self.linecolors = 0
        self.other = ""

        options = options.replace (',', ' ')
        for o in options.split():
            lo = o.lower()
            if lo in ["merge", "same", "norm", "logy", 'linecolors']:
                setattr (self, lo, 1)
            elif (self._optmatch (lo, "fill") or
                  self._optmatch (lo, "linetype") or
                  self._optmatch (lo, "color")):
                pass
            else:
                self.other += o
        return


    def _optmatch (self, lo, pat):
        if lo.startswith (pat+'='):
            setattr (self, pat, int (lo[len (pat)+1:]))
            return 1
        return 0
    

def draw_obj (obj, options = "", padnum = -1, pad = None, min=None, max=None):
    """Draw the root object OBJ in the next available pad.

Inputs:
  obj -         The object to draw.
  options -     Drawing options.
                These are passed through to the root Draw
                method, except that we have special
                handling for the SAME option, and add a new MERGE option.
                See the header for details.
  padnum -      If this is a non-negative integer, then this specifies
                the pad in which to draw, overriding
                other specifications except for PAD.  Note: subpad numbers
                start with 1.
  pad -         Explicitly specify the pad to use for drawing.
                Overrides all other specifications.

Returns:
  The object that we drew (may not be the same as OBJ if we
  made a copy).
"""
    global _samecount

    if min is not None:
        obj.SetMinimum (min)
    if max is not None:
        obj.SetMaximum (max)
        
    op = _options (options)

    # Should we advance to the next pad?
    advance_p = 0

    # Should we do vertical axis rescaling?
    rescale_p = 0

    # For SAME and MERGE, we have to pass SAME on to root.
    if op.same or op.merge:
        op.other += "SAME"

    if not op.same:
        # No SAME option.  Reset the count.
        _samecount = 0

        # Advance to the next pad.
        advance_p = 1
    else:
        # SAME was specified.  Keep count of the number of such.
        _samecount += 1
        rescale_p = 1

    # Handle the MERGE option.
    if op.merge:
        rescale_p = 1
        advance_p = 1

    if pad:
        pad.cd()
    else:
        pad = get_pad (advance_p, padnum)

    if not op.merge and not op.same:
        pad.SetLogy (not not op.logy)

    if isinstance (obj, TH1) and not isinstance (obj, TH2):
        h = obj
        if op.norm:
            h = h.Clone()
            intg =  h.Integral()
            if intg == 0:
                intg = 1
            h.Scale (1. / intg)
            if max is not None:
                h.SetMaximum (max)

        # Special handling for 1D histograms.
        # If SAME was specified, rescale the vertical axis, if needed.
        if rescale_p and max is None:
            # Find the first hist already plotted.
            hfirst = None
            prims = pad.GetListOfPrimitives()
            # Avoids RecursiveRemove crash...
            prims.ResetBit(TObject.kMustCleanup)
            for obj in prims:
                if isinstance (obj, TH1):
                    hfirst = obj
                    break

            # If the new hist's maximum is larger than the first one,
            # adjust the maximum of the first.
            if hfirst and h.GetMaximum() > hfirst.GetMaximum():
                hfirst.SetMaximum (h.GetMaximum() * 1.1)

            if hfirst and h.GetMinimum() < hfirst.GetMinimum():
                hfirst.SetMinimum (h.GetMinimum())

        # Draw a copy of the histogram.
        # Adjust the line style.
        hh = h.DrawCopy (op.other)
        if op.linetype >= 0:
            hh.SetLineStyle (op.linetype)
        else:
            hh.SetLineStyle ((_samecount%4)+1)
        if op.color >= 0:
            hh.SetLineColor (op.color)
        elif op.linecolors and _samecount >= 4:
            hh.SetLineColor (_samecount//4 + 1)
        if op.fill >= 0:
            hh.SetFillColor (op.fill)
        obj = hh
    else:
        # Not a 1-D histogram.  Just draw it.
        obj.Draw (op.other)

    return obj


def get_pad (advance_p = 1, padnum = -1):
    """Advance to the next pad, if requested.
Allow clicking on the pads (button 2) to change the next pad.
"""
    global _lastpad, _nextpad, _npads
    
    c1 = get_canvas()
    pad = TVirtualPad.Pad()

    if advance_p:
        if (pad and
            pad.GetCanvasID() == c1.GetCanvasID() and
            pad.GetNumber() != _lastpad and pad.GetNumber() != 0):
            _nextpad = pad.GetNumber()
            if _nextpad > _npads:
                _nextpad = 1

        # Set up to draw on nextpad.
        # Bump it for the next go around.
        if _npads > 1:
            _lastpad = _nextpad
            _nextpad += 1
            if _nextpad > _npads:
                _nextpad = 1
        else:
            _lastpad = 0

        # Select the pad.
        c1.cd (_lastpad)

    # Handle padnum.
    if padnum >= 0:
        _lastpad = padnum
        if _npads > 0:
            _nextpad = _lastpad + 1
            if _nextpad > _npads:
                _nextpad = 0
        else:
            _nextpad = 0

        c1.cd (_lastpad)

    # Refetch the pad.
    return TVirtualPad.Pad()


_canvas = None
def get_canvas (cname = "c1"):
    """Return the canvas named CNAME.
Create it if it doesn't exist.
"""
    global _canvas
    _canvas = gROOT.FindObject (cname)
    if not _canvas:
        _canvas = TCanvas (cname, cname, 700, 600)
        _canvas.SetLeftMargin (0.15)
        _canvas.SetBottomMargin (0.15)
        _canvas.SetLogy (0)
    return _canvas



def zone (nx, ny):
    """Divide the canvas into subpads.
NX and NY are the number of subpads in x and y, respectively.
"""
    global _npads, _nextpad, _lastpad
    c1 = get_canvas()
    c1.Clear()
    c1.Divide (nx, ny)
    _npads = nx*ny
    _nextpad = 1
    _lastpad = 0
    return


def printeps (s = "out.eps"):
    """Print the current canvas as an eps file."""

    c1 = get_canvas()
    c1.Print (s, "eps,Portrait")
    return
