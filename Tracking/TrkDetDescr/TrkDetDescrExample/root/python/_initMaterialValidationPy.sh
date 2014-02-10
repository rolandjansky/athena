#eosmount ~/eos

echo "oh hai."

base="~/eos/atlas/user/w/wlukas/Material"
g4="AtlasMaterialG4-"
tg="AtlasMaterialTG-"

geo="ATLAS-GEO-16-01-00"

geo16g4old="$base/$geo/$g4$geo.200000.root"
echo "geo16g4old = $geo16g4old"

geo="ATLAS-GEO-18-01-00"

geo18g4new="$base/$geo/$g4$geo.1000000.root"
geo18tgnew="$base/$geo/$tg$geo.1000000.root"
echo "geo18g4new = $geo18g4new"
echo "geo18tgnew = $geo18tgnew"

geo18g4old="$base/$geo/$g4$geo.200000.root"
geo18tgold="$base/$geo/$tg$geo.200000.root"
echo "geo18g4old = $geo18g4old"
echo "geo18tgold = $geo18tgold"

geo="ATLAS-GEO-19-01-00"

geo19g4old="$base/$geo/$g4$geo.2000000.root"
geo19tgold="$base/$geo/$tg$geo.2000000.root"
echo "geo19g4old = $geo19g4old"
echo "geo19tgold = $geo19tgold"

echo "kthxbai."
