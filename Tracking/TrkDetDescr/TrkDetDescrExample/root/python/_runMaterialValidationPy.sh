
#python MaterialValidation.py files="$geo18g4new,$geo18tgnew" trees="TVolume_" params="PathInX0:Eta" saveas="eps" -b
#python MaterialValidation.py files="$geo18g4new,$geo18tgnew" trees="BeamPipe" params="PathInX0:Eta" saveas="eps" -b

#python MaterialValidation.py files="$geo18g4new,$geo18tgnew,$geo18g4old" trees="BeamPipe" params="PathInX0:Eta" saveas="eps" -b

python MaterialValidation.py files="$geo18g4new,$geo18g4old" trees="TVolume" params="PathInX0:Eta" saveas="eps" -b
#python MaterialValidation.py files="$geo18g4new,$geo19g4old" trees="BeamPipe" params="PathInX0:Eta" saveas="eps" -b
#python MaterialValidation.py files="$geo18g4new,$geo16g4old" trees="InDet" params="PathInX0:Eta" saveas="eps" -b

#python MaterialValidation.py files="$geo18g4new" trees="BeamPipe,InDet" params="PathInX0:Eta" saveas="eps" -b
