# 🕯️ Haunted Maze - OpenGL 3D Horror Game

Atmosférická 3D hororová hra vytvořená v C++ s využitím moderního OpenGL. Cílem hráče je posbírat náhodný počet artefaktů (zlatých králíčků) v temném labyrintu, zatímco se musí vyhýbat levitujícím hořícím lebkám.

## 🚀 Hlavní Funkce
- **Dynamické osvětlení:** Implementace Blinn-Phongova modelu s kombinací Directional Light (měsíční svit), Point Lights (záře lebek) a Spot Light (hráčova baterka).
- **Custom Shader Effect:** Králíčci využívají speciální fragment shader logic – v temnotě jsou téměř průhlední (duchové), ale pod přímým světlem baterky se díky dynamické neprůhlednosti (Light-based Opacity) plně zhmotní.
- **Particle System:** Lebky za sebou zanechávají jiskry ohně generované na CPU, které reagují na kolize se stěnami labyrintu.
- **3D Prostorový Zvuk:** Integrace knihovny `miniaudio` pro ambientní podkres, dynamické kroky hráče a prostorové zvuky entit (cinkání králíčků, šepot lebek).
- **Procedurální prvky:** Náhodné generování pozic a počtu nepřátel i artefaktů při každém startu nové hry.
- **Kompletní UI:** Hlavní menu, pauza, obrazovka vítězství a smrti pomocí `Dear ImGui`.

## 🎮 Ovládání
- **W, A, S, D** - Pohyb v bludišti
- **Myš** - Rozhlížení
- **Mezerník** - Skok
- **F** - Zapnutí/Vypnutí baterky
- **ESC** - Menu / Pauza
- **F3** - Zobrazení Debug HUD (FPS, pozice)

## 🛠️ Technologie
- **Jazyk:** C++ 
- **Grafické API:** OpenGL 4.6
- **Knihovny:**
  - `GLFW` & `Glad` (správa oken a kontextu)
  - `GLM` (matematika)
  - `Assimp` (načítání 3D modelů)
  - `Dear ImGui` (uživatelské rozhraní)
  - `miniaudio` (audio engine)

## 📦 Instalace a spuštění (Release)
1. Přejděte do sekce Releases.
2. Stáhněte si přiložený `.zip` soubor.
3. Rozbalte obsah kamkoliv do složky.
4. Spusťte `Engine3D.exe`.

> **Poznámka:** Pro správný běh je nutné zachovat složku `assets/` ve stejném adresáři jako spustitelný soubor.

## 📂 Struktura projektu
- `src/` - Zdrojové kódy (`main.cpp`, logika hry)
- `include/` - Hlavičkové soubory a vlastnoruční moduly (Camera, Audio, Shader...)
- `assets/`
  - `models/` - 3D modely
  - `textures/` - Textury stěn a podlah
  - `shaders/` - Vertex a Fragment shadery v GLSL
  - `audio/` - Hudba a zvukové efekty
