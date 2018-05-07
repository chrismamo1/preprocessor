#!/usr/bin/fish
# example: to use 5 clusters to polarize an entire directory, try this:
# $ ./preprocess.fish -m polarize --clusters 5 (find /some/path/to/process/ -type f)
# it is worth noting that, at least in my handful of contrived tests, it's actually
# almost always better to use 2 clusters.

set -l mode 'polarize'
set -l files
set -l params
set -l oDir 'out'
while set -q argv[1]
  switch $argv[1]
    case '-h' '--help'
      echo 'Usage: ' $argv[1] '[-h | --help]' '[(-m | --mode) (polarize | detect edges | detect features)]' \
        '[(-o | --odir) default is \'out\']'
      echo 'Options for polarization:'
      echo '\t(-c | --clusters)\tnumber of clusters to use in KMeans'
      exit 0
    case '-c' '--clusters'
      set params $params '-c' $argv[2]
      set -e argv[1 2]
    case '-m' '--mode'
      set params $params '-x' $argv[2]
      set -e argv[1 2]
    case '-o' '--odir'
      set oDir $argv[2]
      set -e argv[1 2]
    case '*'
      set files $files $argv[1]
      set -e argv[1]
  end
end

for i in $files
  set tName (mktemp);
  convert $i $tName.ppm;
  #set generatedFiles (./main -x 'detect edges' -c 2 -k 3 $tName.ppm out/$i.ppm);
  #set generatedFiles (./main -c 2 -k 3 $tName.ppm out/(basename $i).ppm);
  set generatedFiles (./main $params -k 3 $tName.ppm $oDir/(basename $i).ppm)
  for j in $generatedFiles
    set_color purple
    echo "Handling preprocessor output file " $j
    set_color normal
    convert $j $j.png
    rm $j
  end
  rm $tName;
  rm $tName.ppm;
end