#!/bin/bash
set -e

SOURCE="${BASH_SOURCE[0]}"
while [ -h "${SOURCE}" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "${SOURCE}" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ ${SOURCE} != /* ]] && SOURCE="${DIR}/${SOURCE}" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR="$( cd -P "$( dirname "${SOURCE}" )" && pwd )"


WINE=wineconsole
if [ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ]; then
    WINE=
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW64_NT" ]; then
    WINE=
fi

if [ $# -ne 2 ]; then
  echo "$0 source destination"
  exit 1
fi

decrypted_file=`mktemp`
echo "Temp file $decrypted_file"

echo "Seeking for 71863.exml ..."
seven=$(find "$1" -name 71863.exml)
if [ $? -ne 0 ]; then
  echo "71863.exml not found"
  exit 1
fi
echo "71863.exml    $seven"

echo "Extracting key ..."
${DIR}/exml "$seven" > ${decrypted_file}
key="Fo4dS9X$(xmllint --xpath 'string(/MCPTimeout/ValueStore[@Name="CM_ENCRYPTION"]/@Value)' ${decrypted_file})"
echo "Key    ${key}"

function decrypt() {
  key_file=`mktemp`
  printf "${4}" > "${key_file}"
  decrypt_file "${1}" "${2}" "${3}" "${key_file}"
}

function decrypt_file() {
  local dir=$(dirname "${3}")
  local filename=$(basename "${3}")
  local rdir=$(python -c "from __future__ import print_function; import os.path; print(os.path.relpath('${dir}', '${1}'))")
  local cdir="${2}/${rdir}"
  local outfile=${cdir}/${filename}
  mkdir -p "${cdir}"
  if ${DIR}/decrypt "${4}" "${3}" > "${outfile}"; then
    if file --mime-type "${outfile}" | grep -q "zip"; then
      echo "Extracting ${outfile} ..."
      unzip "${outfile}" -d "${cdir}" > /dev/null
      rm "${outfile}"
    fi
  else
    echo "Error decrypting ${3}"
  fi
}

function unzip_file() {
  local dir=$(dirname "${3}")
  local filename=$(basename "${3}")
  local rdir=$(python -c "from __future__ import print_function; import os.path; print(os.path.relpath('${dir}', '${1}'))")
  local cdir="${2}/${rdir}"
  mkdir -p "${cdir}"
  unzip "${3}" -d "${cdir}" > /dev/null
}

find "${1}" -type f -exec sh -c "head -c 8 '{}' | grep Salted__ > /dev/null 2>&1" \; -print0 |
  while IFS= read -r -d $'\0' line; do
    decrypt "${1}" "${2}" "${line}" "${key}"
  done

echo "Seeking for EngineeringFeedbackConfig.exml ..."
eng=$(find "${1}" -name EngineeringFeedbackConfig.exml)
if [ $? -ne 0 ]; then
  echo "EngineeringFeedbackConfig.exml not found"
  exit 1
fi
echo "EngineeringFeedbackConfig.exml    ${eng}"
decrypt "${1}" "${2}" "${eng}" '3B57C2EA-0C12-4062-852F-DE4B7F5D71D7'

echo "Seeking for Fnpss.dll ..."
fnpssdll=$(find "$1" -name Fnpss.dll)
if [ $? -ne 0 ]; then
  echo "Fnpss.dll not found"
  exit 1
fi
echo "Fnpss.dll    ${fnpssll}"
echo "Seeking for fnpss.ds ..."
fnpss=$(find "${1}" -name fnpss.ds)
if [ $? -ne 0 ]; then
  echo "fnpss.ds not found"
  exit 1
fi
echo "fnpss.ds    ${fnpss}"
runtime=`dirname "${fnpssdll}"`
echo "Runtime directory ${runtime}"
key_file=`mktemp`
$WINE $DIR/fnp "${runtime}" "${key_file}"
decrypt_file "${1}" "${2}" "${fnpss}" "${key_file}"


echo "Seeking for xmlfiles.enc ..."
xmlfiles=$(find "${1}" -name xmlfiles.enc)
if [ $? -ne 0 ]; then
  echo "xmlfiles.enc not found"
  exit 1
fi
echo "xmlfiles.enc    $xmlfiles"
# Key getting from IDS/Runtime/NGImporter.exe
decrypt "${1}" "${2}" "${xmlfiles}" '3#l@$Btx_9S@jrT+EBvD[17ku9B='

xml_text=$(find "${1}" -name xml_text.zip)
echo "Extracting ${xml_text} ..."
unzip_file "${1}" "${2}" "${xml_text}"
