# Use
# sh run_plots_photons.sh path_to_root_file

dataset=$1
ds=${dataset##*_}
#echo $ds
ds=${ds%.root}
#echo $ds
mkdir plots
python plot_Perf_Eff_photons.py -b $dataset
gs -dBATCH -dNOPAUSE -q -sDEVICE=pdfwrite -sOutputFile=Perf_Eff_${ds}.pdf plots/plot_*
rm plots/*