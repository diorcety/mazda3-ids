#!/bin/bash
set -e

SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"


if [ $# -ne 2 ]; then
  echo "$0 source destination"
  exit 1
fi

echo "Seeking for 71863.exml ..."
seven=$(find "$1" -name 71863.exml)
if [ $? -ne 0 ]; then
  echo "71863.exml not found"
  exit 1
fi
echo "71863.exml    $seven"

echo "Extracting key ..."
decrypted_file=`mktemp`
$DIR/exml "$seven" > $decrypted_file
key="Fo4dS9X$(xmllint --xpath 'string(/MCPTimeout/ValueStore[@Name="CM_ENCRYPTION"]/@Value)' $decrypted_file)"
echo "Key    $key"


function decrypt() {
        dir=$(dirname "$3")
        rdir=$(python -c "import os.path; print os.path.relpath('$dir', '$1')")
	cdir="$2/$rdir"
	mkdir -p "$cdir"
	echo "Extracting $3..."
	if $DIR/decrypt "$4" "$3" > $decrypted_file; then
		unzip $decrypted_file -d "$cdir" > /dev/null
	else
		echo "Error extracting $3"
	fi
}


find "$1" -type f -exec sh -c "head -c 8 '{}' | grep Salted__ > /dev/null 2>&1" \; -print0 | 
    while IFS= read -r -d $'\0' line; do
	decrypt "$1" "$2" "$line" "$key"
    done

echo "Seeking for EngineeringFeedbackConfig.exml ..."
eng=$(find "$1" -name EngineeringFeedbackConfig.exml)
if [ $? -ne 0 ]; then
  echo "EngineeringFeedbackConfig.exml not found"
  exit 1
fi
echo "EngineeringFeedbackConfig.exml    $eng"
decrypt "$1" "$2" "$eng" '3B57C2EA-0C12-4062-852F-DE4B7F5D71D7'

echo "Seeking for fnpss.ds ..."
fnpss=$(find "$1" -name fnpss.ds)
if [ $? -ne 0 ]; then
  echo "fnpss.ds not found"
  exit 1
fi
echo "fnpss.ds    $fnpss"
decrypt "$1" "$2" "$fnpss" '7E2162B9-DB92-4987-83F0-018B0873357B'


echo "Seeking for xmlfiles.enc ..."
xmlfiles=$(find "$1" -name xmlfiles.enc)
if [ $? -ne 0 ]; then
  echo "xmlfiles.enc not found"
  exit 1
fi
echo "xmlfiles.enc    $fnpss"
decrypt "$1" "$2" "$xmlfiles" '3#l@$Btx_9S@jrT+EBvD[17ku9B='

