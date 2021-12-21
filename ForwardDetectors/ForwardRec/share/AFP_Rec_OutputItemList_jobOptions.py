AFP_ItemList=[]


# AFP Silicon hits containers
AFP_ItemList.append("xAOD::AFPSiHitContainer#AFPSiHitContainer")
AFP_ItemList.append("xAOD::AFPSiHitAuxContainer#AFPSiHitContainerAux.")
AFP_ItemList.append("xAOD::AFPSiHitsClusterContainer#AFPSiHitsClusterContainer")
AFP_ItemList.append("xAOD::AFPSiHitsClusterAuxContainer#AFPSiHitsClusterContainerAux.")

# for runs with more BCX ids i.e. before timing in the detectors
for bcIDshift in range (1, 6):
    name = "AFPSiHitContainerBcIDplus" + str(bcIDshift)
    AFP_ItemList.append("xAOD::AFPSiHitContainer#" + name)
    AFP_ItemList.append("xAOD::AFPSiHitAuxContainer#" + name + "Aux.")

AFP_ItemList.append("xAOD::AFPTrackContainer#AFPTrackContainer")
AFP_ItemList.append("xAOD::AFPTrackAuxContainer#AFPTrackContainerAux.")
AFP_ItemList.append("xAOD::AFPToFHitContainer#AFPToFHitContainer")
AFP_ItemList.append("xAOD::AFPToFHitAuxContainer#AFPToFHitContainerAux.")
AFP_ItemList.append("xAOD::AFPToFTrackContainer#AFPToFTrackContainer")
AFP_ItemList.append("xAOD::AFPToFTrackAuxContainer#AFPToFTrackContainerAux.")
