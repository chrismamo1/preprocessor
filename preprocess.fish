#!/usr/bin/fish

for i in $argv
  set tName (mktemp);
  convert $i $tName.ppm;
  ./main -c 2 -d 2 -s 1.1 -k 3 -l 100 $tName.ppm out/$i.ppm;
  convert out/$i.ppm out/$i.ppm.png;
  rm $tName;
  rm $tName.ppm;
  rm out/$i.ppm;
end