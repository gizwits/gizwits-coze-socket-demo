from pydub import AudioSegment
import sys
import os

def trim_audio(input_file, output_file, duration_ms=3000):
    try:
        # 加载音频文件
        audio = AudioSegment.from_mp3(input_file)
        
        # 提取前500ms
        trimmed_audio = audio[:duration_ms]
        
        # 导出文件
        trimmed_audio.export(output_file, format="mp3")
        
        print(f"Successfully trimmed {input_file} to {duration_ms}ms")
        print(f"Output saved to: {output_file}")
        
    except Exception as e:
        print(f"Error processing file: {str(e)}")
        sys.exit(1)

def main():
    if len(sys.argv) < 2:
        print("Usage: python trim_audio.py <input_file> [output_file]")
        sys.exit(1)
    
    input_file = sys.argv[1]
    
    # 如果没有指定输出文件名，自动生成
    if len(sys.argv) > 2:
        output_file = sys.argv[2]
    else:
        # 从输入文件名生成输出文件名
        filename, ext = os.path.splitext(input_file)
        output_file = f"{filename}_500ms{ext}"
    
    # 检查输入文件是否存在
    if not os.path.exists(input_file):
        print(f"Error: Input file '{input_file}' does not exist")
        sys.exit(1)
    
    trim_audio(input_file, output_file)

if __name__ == "__main__":
    main()