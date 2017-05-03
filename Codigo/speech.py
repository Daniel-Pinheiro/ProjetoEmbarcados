import speech_recognition as sr

rec = sr.Recognizer()

while True:
    with sr.Microphone() as s:
        rec.adjust_for_ambient_noise(s)
        print('Diga algo')
        
        audio = rec.listen(s)
        print('Você disse: ' + rec.recognize_sphinx(audio) )
