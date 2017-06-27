import requests
import speech_recognition as sr

#URL_KODI = "http://192.168.1.10:8080/kodiCmds/kodiHttp/?command="
URL_KODI = "http://localhost:8080/jsonrpc?request={\"jsonrpc\": \"2.0\", \"method\": "	

r = sr.Recognizer()

while True:

	with sr.Microphone() as source:
		print("Waiting...")
		audio = r.listen(source)

	try:
		command = r.recognize(audio)

		if "Hey Ultron" in command.lower():
			print("Hi, my name is Ultron. How can I help you?")

			with sr.Microphone() as source:
				audio = r.listen(source)

			try:
				command = r.recognize(audio)

				if "Play" in command.lower():
					COMMAND_KODI = "\"Player.PlayPause\", \"params\": { \"playerid\": 0 }, \"id\": 1}"

				elif "Pause" in command.lower():
					COMMAND_KODI = "\"Player.PlayPause\", \"params\": { \"playerid\": 0 }, \"id\": 1}"

				elif "Stop" in command.lower():
					COMMAND_KODI = "\"Player.Stop\", \"params\": { \"playerid\": 1 }, \"id\": 1}"
				
				elif "Initial Screen" in command.lower():
					COMMAND_KODI = "\"Input.Home\"}"
				
				elif "Up" in command.lower():
					COMMAND_KODI = "\"Input.Up\"}"

				elif "Down" in command.lower():
					COMMAND_KODI = "\"Input.Down\"}"

				elif "Left" in command.lower():
					COMMAND_KODI = "\"Input.Left\"}"

				elif "Right" in command.lower():
					COMMAND_KODI = "\"Input.Right\"}"

				elif "Select" in command.lower():
					COMMAND_KODI = "\"Input.Select\"}"

				elif "Back" in command.lower():
					COMMAND_KODI = "\"Input.Back\"}"

				elif "Next" in command.lower():
					COMMAND_KODI = "\"Player.GoTo\",\"id\":1,\"params\":{\"playerid\":1,\"to\":\"next\"}}"

				elif "Previous" in command.lower():
					COMMAND_KODI = "\"Player.GoTo\",\"id\":1,\"params\":{\"playerid\":1,\"to\":\"previous\"}}" 

				elif "Shutdown" in command.lower():
					break

			except LookupError:     
				print("Command inexistent")

			
			req = requests.get(URL_KODI + COMMAND_KODI)
		
	except LookupError:
		print("Sorry, you need to say 'Hey Ultron' first. Try again")	
