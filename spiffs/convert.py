from pydub import AudioSegment
import os
import sys

def convert_audio(input_file, output_file):
    try:
        # 加载音频文件
        audio = AudioSegment.from_mp3(input_file)
        
        # 转换为单声道
        audio = audio.set_channels(2)
        
        # 设置采样率为 16kHz
        audio = audio.set_frame_rate(16000)
        
        # 导出文件
        audio.export(output_file, format="mp3")
        
        print(f"Successfully converted {input_file} to {output_file}")
        print(f"New audio properties:")
        print(f"- Channels: {audio.channels}")
        print(f"- Sample rate: {audio.frame_rate} Hz")
        
    except Exception as e:
        print(f"Error converting file: {str(e)}")
        sys.exit(1)

def main():
    # 检查命令行参数
    if len(sys.argv) != 3:
        print("Usage: python convert.py input.mp3 output.mp3")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    
    # 检查输入文件是否存在
    if not os.path.exists(input_file):
        print(f"Input file {input_file} does not exist")
        sys.exit(1)
    
    convert_audio(input_file, output_file)

if __name__ == "__main__":
    main()