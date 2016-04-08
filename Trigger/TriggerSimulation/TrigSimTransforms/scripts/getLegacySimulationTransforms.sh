echo "Getting trigger simulation transforms"
get_files -symlink -scripts runLegacySimulation_digitization.sh
get_files -symlink -scripts runLegacySimulation_rdo2bs.sh
get_files -symlink -scripts runLegacySimulation_trigsim.sh

get_files -symlink -jo printRODVersions.py
