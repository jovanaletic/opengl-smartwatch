# OpenGL SmartWatch

Projekat pametnog sata napravljen u OpenGL-u za kurs Računarska grafika, FTN Novi Sad.

## 🎯 Funkcionalnosti
- Prikaz vremena sa digitalnim ciframa
- EKG animacija sa animiranim srcem
- Indikator baterije
- Navigacija između ekrana (strelice levo/desno)
- Shader-based rendering

## 🚀 Kako pokrenuti projekat (VAŽNO!)

### Korak 1: Kloniraj repozitorijum
```bash
git clone https://github.com/jovanaletic/opengl-smartwatch.git
cd opengl-smartwatch
```

### Korak 2: Otvori projekat u Visual Studio
- Otvori `SmartWatch.sln` fajl

### Korak 3: Restore NuGet Packages (OBAVEZNO!)
Visual Studio će automatski detektovati nedostajuće pakete:
- Pojaviće se **žuto obaveštenje** na vrhu: "Some NuGet packages are missing"
- Klikni na **"Restore"** dugme
- Sačekaj 10-20 sekundi da se preuzmu paketi

**Alternativno (ako ne vidiš obaveštenje):**
- Desni klik na Solution u Solution Explorer → "Restore NuGet Packages"

**Ili koristi Package Manager Console:**
- Tools → NuGet Package Manager → Package Manager Console
- Otkucaj: `Update-Package -reinstall`

### Korak 4: Build i Run
- Build: `Ctrl+Shift+B`
- Run: `F5`

## ⚠️ Česte greške

### LNK1104: cannot open file 'glew32.lib'
**Uzrok:** NuGet packages nisu instalirani.
**Rešenje:** Vrati se na Korak 3 i restore-uj pakete.

### Missing DLL errors
**Uzrok:** Projekat nije build-ovan.
**Rešenje:** Ctrl+Shift+B pa ponovo F5.

## 🛠️ Tehnologije
- **Jezik:** C++
- **Graphics:** OpenGL 3.3+
- **Biblioteke:**
  - GLEW 2.2.0 (OpenGL Extension Wrangler)
  - GLFW 3.4.0 (Window management)
  - STB Image (Texture loading)

## 📁 Struktura projekta
```
opengl-smartwatch/
├── Source/          # Main source fajlovi
├── Header/          # Header fajlovi
├── Resources/       # Slike i teksture
├── Callbacks.cpp    # Event handlers
├── Drawing.cpp      # Rendering logika
├── *.vert, *.frag   # Shader fajlovi
└── SmartWatch.sln   # Visual Studio solution
```

## 👤 Autor
Projekat za kurs Računarska grafika, FTN Novi Sad

## 📝 Napomena
Paketi (`packages/` folder) se **ne čuvaju u Git repozitorijumu**. Svaki developer mora da ih preuzme preko NuGet-a prilikom prvog pokretanja projekta.