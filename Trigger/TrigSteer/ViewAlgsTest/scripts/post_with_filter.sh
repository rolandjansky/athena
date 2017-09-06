
test=$1
log=$1.log
echo "patterns" $PATTERNS

filter="grep -e ${PATTERNS// / -e }"
${filter} ${log} > relevant_lines_from_execution_${test}.log
${filter} ../share/${test}.ref > relevant_lines_from_reference_${test}.log

echo "< ref"
echo "> new"

diff relevant_lines_from_reference_${test}.log relevant_lines_from_execution_${test}.log
status=$?
echo "DIFF status" ${status}
if [ ${status} = 1 ]
then
    echo ".. new output differ from the reference"
    echo ".. if expected update reference by copying the file : " relevant_lines_from_reference_${log} "as ${test}.ref"
fi

exit ${status}
