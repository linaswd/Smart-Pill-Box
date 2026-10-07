import serial
import datetime
import time
import requests

PORT = "COM7"
BAUD = 9600
TELEGRAM_TOKEN   = "8920227958:AAFEReASD52MWIOM0rf9LeKhEeLaO7u0Yfk"
TELEGRAM_CHAT_ID = "8302513166"

def send_telegram(message):
    url = f"https://api.telegram.org/bot{TELEGRAM_TOKEN}/sendMessage"
    requests.post(url, data={"chat_id": TELEGRAM_CHAT_ID, "text": message})

ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)
print("Connecte a l'Arduino")

JOURS = ["Lun","Mar","Mer","Jeu","Ven","Sam","Dim"]
last_sync = 0

while True:
    line = ser.readline().decode("utf-8").strip()

    if line == "NEED_TIME" or time.time() - last_sync > 60:
        now = datetime.datetime.now()
        jour = JOURS[now.weekday()]
        data = now.strftime("%H:%M:%S %d/%m/%Y ") + jour
        ser.write((data + "\n").encode())
        print(f"Envoye : {data}")
        last_sync = time.time()

    elif line == "TIME_OK":
        print("Arduino a recu l'heure")

    elif line.startswith("ALERTE"):
        parts = line.split(":")
        slot = parts[1] if len(parts) > 1 else "inconnu"
        now = datetime.datetime.now()
        date_str = now.strftime("%d/%m/%Y")
        heure_str = now.strftime("%H:%M")
        msg = (f"Smart Pill Box\n"
               f"Date : {date_str}\n"
               f"Heure : {heure_str}\n"
               f"Medicament non pris ! Slot : {slot}\n"
               f"Veuillez verifier le patient.")
        send_telegram(msg)
        print(f"Alerte Telegram envoyee : {msg}")

    elif line:
        print(f"Arduino -> {line}")