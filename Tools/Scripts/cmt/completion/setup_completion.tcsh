
complete athena 'p/*/`lookup_joboptions.sh`/'
complete athena.exe 'p/*/`lookup_joboptions.sh`/'
complete athena.py 'p/*/`lookup_joboptions.sh`/'
complete get_joboptions 'p/*/`lookup_joboptions.sh`/'
complete get_txtjoboptions 'p/*/`lookup_txtjoboptions.sh`/'
complete get_pyjoboptions 'p/*/`lookup_joboptions.sh`/'
complete check_joboptions 'p/*/`lookup_txtjoboptions.sh`/'

complete get_files 'p/-1/(-jo -scripts -data -xmls -symlink -copy)/' \
  'n/-jo/`lookup_joboptions.sh`/' \
  'n/-scripts/`lookup_scripts.sh`/' \
  'n/-data/`lookup_data.sh`/' \
  'n/-xmls/`lookup_xmls.sh`/' \
  'N/-jo/`lookup_joboptions.sh`/' \
  'N/-scripts/`lookup_scripts.sh`/' \
  'N/-data/`lookup_data.sh`/' \
  'N/-xmls/`lookup_xmls.sh`/' \
  'c/-/(jo scripts data xmls symlink copy)/' \
  'p/*/(-jo -scripts -data -xmls -symlink -copy)/'


