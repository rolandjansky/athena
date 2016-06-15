"""
Visualize envelopes as defined in EnvelopeDefSvc
Elmar Ritsch, 13/05/2016
"""

from AthenaCommon.CfgGetter import getService


def draw_envelope(ax, rlist_half, zlist_half, color, label):

    # mirror the given (r,z) values in the x-y plane
    rlist = rlist_half[:]
    zlist = zlist_half[:]
    for r in reversed(rlist_half):
        rlist.append(r)
    for z in reversed(zlist_half):
        zlist.append(-z)
    rlist.append(rlist[0])
    zlist.append(zlist[0])

    area = Polygon(zip(zlist, rlist), alpha=0.5, closed=True, color=color, label=label, linewidth=0.)
    ax.add_patch(area)


if __name__ == "__main__":
    import matplotlib
    matplotlib.use('Qt4Agg') # use backend that supports GUI (rather than file output only)
    import matplotlib.pyplot as plt
    from matplotlib.patches import Polygon

    svc = getService('AtlasGeometry_EnvelopeDefSvc')

    fig, ax = plt.subplots(figsize=(20,10))

    draw_envelope(ax, svc.FallbackBeamPipeR, svc.FallbackBeamPipeZ, 'r', label='BeamPipe')
    draw_envelope(ax, svc.FallbackInDetR, svc.FallbackInDetZ, 'b', label='InDet')
    draw_envelope(ax, svc.FallbackCaloR, svc.FallbackCaloZ, 'c', label='Calo')
    draw_envelope(ax, svc.FallbackMuonR, svc.FallbackMuonZ, 'm', label='MS')
    draw_envelope(ax, svc.FallbackCavernR, svc.FallbackCavernZ, 'k', label='Cavern')

    ax.autoscale_view()
    plt.ylabel('r  (mm)')
    plt.xlabel('z  (mm)')
    plt.legend()
    plt.show(block=True)
    exit(1)
