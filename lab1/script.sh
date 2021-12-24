#!/bin/bash

if [ -z $1 ]; then
  echo file name not specified
  exit 1
fi

string_from_file_output="$(grep -i "Output:" $1)"
string_from_file_output_with_whitespace=${string_from_file_output#///Output:}
file_name_from_output=$(echo "$string_from_file_output_with_whitespace" | tr -d '[:space:]')

if [ -z "$file_name_from_output" ]; then
  echo filename not found
  exit 1
fi

temp_folder=$(mktemp -d)

trap "rm -Rf $temp_folder; exit 1" SIGHUP SIGINT SIGQUIT SIGTERM
trap "rm -Rf $temp_folder" EXIT

cp "$1" "$temp_folder"/

current_dir=$(pwd)

cd "$temp_folder"

gcc "$1" -o "$file_name_from_output"

if [ $? -ne 0 ]; then
  echo Compilation failed!
  cd "$current_dir"
  exit 1
fi

mv "$file_name_from_output" "$current_dir"
cd "$current_dir"
echo Building: "$file_name_from_output"
exit 0
