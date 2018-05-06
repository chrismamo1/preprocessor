#!/usr/bin/fish

for i in $argv
  set tName (mktemp);
  convert $i $tName.ppm;
  #set generatedFiles (./main -x 'detect edges' -c 2 -d 2 -s 1.1 -k 3 -l 100 $tName.ppm out/$i.ppm);
  set generatedFiles (./main -c 2 -d 2 -s 1.1 -k 3 -l 100 $tName.ppm out/(basename $i).ppm);
  for j in $generatedFiles
    echo "j = " %j;
    convert $j $j.png
    rm $j
  end
  rm $tName;
  rm $tName.ppm;
end