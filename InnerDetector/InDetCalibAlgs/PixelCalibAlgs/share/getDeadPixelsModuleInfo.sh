#!/bin/zsh

pixelList=DeadPixels.txt

read_module=0
read_pixels=0
module=""
pixels=-1
n_pix_med=0
n_pix_bad=0
others=0

for mod in $(cat $pixelList)
do

	if [ $mod = "Module" ]; then 
		read_module=1
	continue
	fi

	if [ $read_module -eq 1 ]; then
		module=$mod
		read_module=0
		continue
	fi

	if [ $mod = "dead" ]; then 
		continue
	fi

	if [ $mod = "pixels" ]; then 
		read_pixels=1
		continue
	fi

	if [ $read_pixels -ge 0 ]; then
		pixels=$mod
		read_pixels=0
		
		#echo "ciao2"
	
		if [ $pixels -ge 46080 ]; then
			disabled_modules=(${disabled_modules[@]-} $(echo "$module"))
			disabled_modules_npix=(${disabled_modules_npix[@]-} $(echo "$pixels"))		
		elif [ $pixels -ge 2000 ]; then
			bad_chip_modules=(${bad_chip_modules[@]-} $(echo "$module"))
			bad_modules_npix=(${bad_modules_npix[@]-} $(echo "$pixels"))
			n_pix_bad=$(($n_pix_bad+$pixels))
		elif [ $pixels -lt 2000 ]; then
				if [ $pixels -ge 100 ]; then
					med_modules=(${med_modules[@]-} $(echo "$module")) 
					med_modules_npix=(${med_modules_npix[@]-} $(echo "$pixels"))
					n_pix_med=$(($n_pix_med+$pixels))
				else others=$(($others+$pixels))
			fi
		fi

	fi

done

echo "Candidate disabled modules ${#disabled_modules[@]}"
i=0
while [ $i -lt ${#disabled_modules[@]} ]; do
	echo "$disabled_modules[$i] $disabled_modules_npix[$i]" 
	i=$(($i+1))
done

echo "Candidate modules with a bad chip ${#bad_chip_modules[@]}, total pixels $n_pix_bad"
i=0
while [ $i -lt ${#bad_chip_modules[@]} ]; do
	echo "$bad_chip_modules[$i] $bad_modules_npix[$i]"
	i=$(($i+1)) 
done

echo "Modules with medium dead pixels ${#med_modules[@]}"
i=0
while [ $i -lt ${#med_modules[@]} ]; do
	echo "$med_modules[$i] $med_modules_npix[$i]"
	i=$(($i+1)) 
done

echo "Total of pixels on med pix $n_pix_med" 
echo "Others $others" 


disabled_modules=""
bad_chip_modules=""
med_modules=""
disabled_modules_npix=""
bad_modules_npix=""
med_modules_npix=""

unset disabled_modules
unset bad_chip_modules
unset med_modules
unset disabled_modules_npix
unset bad_modules_npix
unset med_modules_npix

