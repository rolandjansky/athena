
```mermaid
graph TD;
subgraph BTagAlgs [BTagAlgs]

assoc[JetParticleAssociation] --> jettag & finder

assocm[JetParticleAssociation muons] --> jettag

finder[JetSecVtxFinding] --> vx[JetSecVertexing]
vx --> jettag

jettag[JetBTagging] ==> muaug[BTagMuonAugmenter] & jetaug[BTagJetAugmenter]
muaug & jetaug ==> dl[Machine Learning Algorithms]
end

db[Calibration Database] --> jettag
tr(tracks) -.-> aug & vx & jetaug & dl
mu(Muons) -.-> assocm & muaug
aug[BTagTrackAugmenter] --> assoc
jet(jets) -.-> assoc & assocm & finder & vx & jettag
pv(Primary Vertex) -.-> finder & vx & jettag
json(List of NN Files) -.-> dl
dl ==> btag(BTagging Object)
```

This is a rough sketch of how information flows through the b-tagging code. Little boxes are algorithms, and the dotted lines indicate where some information needs to be passed into them. Solid lines indicate objects that are created internally. In the solid line case, the name of the object has to be synchronized between algorithms. The `BTagging` object follows the thick black line.

Many of the functions to configure the above can be found in this
package, with `Cfg` appended to the name. In particular, the main
function here is `BTagAlgsCfg`, which can be found in
[BTagRun3Config](python/BTagRun3Config.py).
