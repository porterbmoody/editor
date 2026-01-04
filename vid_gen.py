from moviepy.editor import ImageClip, AudioFileClip

image_path = "future.jpg"
audio_path = "warp3.wav"
output_path = "output_video.mp4"

audio_clip = AudioFileClip(audio_path)
image_clip = ImageClip(image_path).set_duration(audio_clip.duration)

video_clip = image_clip.set_audio(audio_clip)

video_clip.write_videofile(output_path, fps=24)
