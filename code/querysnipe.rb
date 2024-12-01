#!/usr/bin/env ruby

def main(path, term)
  input = File.read(path)
  input.lines.each do |line|
    if line.include? term
      puts line.gsub!(term, "\x1b[35;1m" + term + "\x1b[0m")
    end
  end
end

if __FILE__ == $0
  if ARGV.length < 2
    puts "Expected at least two arguments."
    puts "Usage: querysnipe PATH TERM"
    exit 1
  end

  main(ARGV[0], ARGV[1])
end
