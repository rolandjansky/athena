AFP_ItemList=[]

AFP_ItemList.append("AFP_TDSimHitCollection#AFP_TDSimHitCollection")
AFP_ItemList.append("AFP_SIDSimHitCollection#AFP_SIDSimHitCollection")

AFP_ItemList.append("AFP_TDDigiCollection#AFP_TDDigiCollection")
AFP_ItemList.append("AFP_SiDigiCollection#AFP_SiDigiCollection")
AFP_ItemList.append("AFP_TDLocRecoEvCollection#AFP_TDLocRecoEvCollection")
AFP_ItemList.append("AFP_SIDLocRecoEvCollection#AFP_SIDLocRecoEvCollection")

# AFP Silicon hits containers
AFP_ItemList.append("xAOD::AFPSiHitContainer#AFPSiHitContainer")
AFP_ItemList.append("xAOD::AFPSiHitAuxContainer#AFPSiHitContainerAux.")
# for runs with more BCX ids i.e. before timing in the detectors
for bcIDshift in range (1, 6):
    name = "AFPSiHitContainerBcIDplus" + str(bcIDshift)
    AFP_ItemList.append("xAOD::AFPSiHitContainer#" + name)
    AFP_ItemList.append("xAOD::AFPSiHitAuxContainer#" + name + "Aux.")

AFP_ItemList.append("xAOD::AFPTrackContainer#AFPTrackContainer")
AFP_ItemList.append("xAOD::AFPTrackAuxContainer#AFPTrackContainerAux.")
AFP_ItemList.append("xAOD::AFPToFHitContainer#AFPToFHitContainer")
AFP_ItemList.append("xAOD::AFPToFHitAuxContainer#AFPToFHitContainerAux.")

