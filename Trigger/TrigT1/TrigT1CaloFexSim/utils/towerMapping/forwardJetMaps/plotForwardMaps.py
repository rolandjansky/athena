
filenames = {
    'seeds localMax_ge': 'searchGe_*.dat',
    'seeds localMax_gt': 'searchG_*.dat',
    'towers in seed': 'seed_*.dat',
    'towers in jet': 'jet_*.dat',
    }

order = ['seeds localMax_ge', 'seeds localMax_gt', 'towers in seed', 'towers in jet']

jetRadius = 0.4
seedSize = 0.3

markers = {
    'seeds localMax_ge': 'D',
    'seeds localMax_gt': 's',
    'towers in seed': 'o',
    'towers in jet': '^',
    }

sizes = {
    'seeds localMax_ge': 6,
    'seeds localMax_gt': 7,
    'towers in seed': 5,
    'towers in jet': 2,
    }

njets = 9999

from glob import glob
lines = {}
for key in filenames:
    lines[key] = []
    for filename in sorted(glob(filenames[key])):
        thisfile = open(filename)
        lines[key] += thisfile.readlines()
        thisfile.close()
        
import matplotlib.pyplot as plt
fig, ax = plt.subplots(figsize=(4.8,5.6))


for i_jet in range(len(lines[key])):
    if i_jet >= njets:
        break
    if (i_jet % 10) == 0:
        print "plotting jet", i_jet, "of", min(njets, len(lines[key]))
    line = {}
    for key in lines:
        line[key] = lines[key][i_jet].strip()


    for key in order:
        if key not in line:
            continue

        phis = [float(i) for i in line[key].split()[::2]]
        etas = [float(i) for i in line[key].split()[1::2]]
                
        # for i in range(len(phis)):
            # print phis[i], etas[i]

        ax.plot( etas, phis, markers[key], markersize=sizes[key], label=key)

        if order.index(key) == len(order)-1:
            centre, = ax.plot( etas[0], phis[0], 'x', markersize=4, color='black' )
            # circle radius [jetRadius] around this point
            circle = plt.Circle((etas[0], phis[0]), jetRadius, fill=False)
            ax.add_artist(circle)
            # square side [seedSize] around this point
            rectangle = plt.Rectangle((etas[0]-0.5*seedSize, phis[0]-0.5*seedSize), seedSize, seedSize, fill=False )
            ax.add_artist(rectangle)

    ax.set_xlim(etas[0]-jetRadius*1.2, etas[0]+jetRadius*1.2)
    ax.set_ylim(phis[0]-jetRadius*1.2, phis[0]+jetRadius*1.6)
    ax.set_xlabel('$\eta$')
    ax.set_ylabel('$\phi$')
    plt.legend(ncol=2)
    plt.savefig('plots/seed' + str(i_jet) + '_EtaPhi.png', bbox_inches="tight", dpi=200)

    centre.remove()
    circle.remove()
    rectangle.remove()
    plt.cla()
