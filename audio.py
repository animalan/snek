def process_text_file(input_file, output_file):
  with open(input_file, 'r') as file:
      lines = file.readlines()

  # Filter lines to remove words shorter than 8 characters and remove "*" characters
  filtered_lines = []
  for line in lines:
      words = line.strip().split()
      filtered_words = []
      for word in words:
          if len(word) >= 8 and "*" not in word:
              filtered_word = "0x" + word
              filtered_words.append(filtered_word)
      filtered_line = ', '.join(filtered_words)
      if filtered_line:  # Append only if there are filtered words in the line
          filtered_line += ','  # Add a comma at the end of the line
          filtered_lines.append(filtered_line)

  # Write filtered lines to output file
  with open(output_file, 'w') as file:
      file.write('\n'.join(filtered_lines))

# File paths
input_file_path = r'input.txt'
output_file_path = r'output.txt'

# Process the text file
process_text_file(input_file_path, output_file_path)
