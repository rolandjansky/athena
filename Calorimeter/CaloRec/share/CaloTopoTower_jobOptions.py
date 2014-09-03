try:
    from CaloRec.CaloTopoTowerGetter import CaloTopoTowerGetter
    CaloTopoTowerGetter()
except Exception:
    treatException("Problem with CaloTopoTower. Switched off")
