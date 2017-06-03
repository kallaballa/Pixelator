cd result
ORIG="$1"

ls -1 *_result.png | while read f; do 
	PREFIX="`echo $f | cut -d"_" -f1`";
  echo $PREFIX
	if [ ! -f ${PREFIX}_frame.png ]; then
			convert ${PREFIX}_result.png ../$ORIG +append ${PREFIX}_merge.png;     
			if [ -f ${PREFIX}_feature.png ]; then
				convert ${PREFIX}_merge.png ${PREFIX}_feature.png -append ${PREFIX}_append.png; 
			else
        convert ${PREFIX}_merge.png ../nomatches.png -append ${PREFIX}_append.png;
			fi
			convert ${PREFIX}_append.png -font Monospace -background none -stroke black -strokewidth 1 -fill white -gravity south -pointsize 28 label:"`echo ${PREFIX} | cut -d"_" -f1`" -composite ${PREFIX}_frame.png
	fi;
done

mencoder mf://*_frame.png -mf w=200:h=200:fps=60:type=png -ovc lavc -lavcopts vcodec=mpeg4:mbd=2:trell -oac copy -o output.avi
