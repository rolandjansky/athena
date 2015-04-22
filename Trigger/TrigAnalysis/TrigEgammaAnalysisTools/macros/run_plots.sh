# Use
# sh run_plots.sh path_to_root_file
dataset=$1
ds=${dataset##*_}
#echo $ds
ds=${ds%.root}
#echo $ds
mkdir plots
python plot_Eff.py -b $dataset
gs -dBATCH -dNOPAUSE -q -sDEVICE=pdfwrite -sOutputFile=Eff_${ds}.pdf plots/plot_Eff_*
rm plots/*
python plot_Perf.py -b $dataset
gs -dBATCH -dNOPAUSE -q -sDEVICE=pdfwrite -sOutputFile=Perf_${ds}.pdf plots/plot_Perf_*
rm -rf plots