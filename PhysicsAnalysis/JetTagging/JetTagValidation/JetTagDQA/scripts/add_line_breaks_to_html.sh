DIR="./testMerging/"

# look for all index.html file in the dir
for FILE in $(find $DIR -name "index.html")
do
  echo $FILE
  # add a break-word to the html at the right point
  sed -i 's/style="text-overflow:ellipsis;overflow:hidden/style="word-wrap:break-word;text-overflow:ellipsis;overflow:hidden/g' $FILE
done