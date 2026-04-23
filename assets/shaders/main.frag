// main.frag
#version 460 core

/**
 * @brief Výstupní barva zpracovávaného fragmentu (pixelu).
 */
out vec4 FragColor;

/**
 * @brief Pozice fragmentu ve světovém prostoru.
 */
in vec3 FragPos;

/**
 * @brief Normálový vektor povrchu v daném fragmentu.
 */
in vec3 Normal;

/**
 * @brief UV souřadnice pro mapování textury.
 */
in vec2 TexCoords;

/**
 * @brief Texturovací jednotka obsahující difuzní mapu.
 */
uniform sampler2D texture1;

/**
 * @brief Základní barva objektu (využita, pokud se nepoužívá textura).
 */
uniform vec3 objectColor;

/**
 * @brief Základní úroveň průhlednosti (alfa kanál).
 */
uniform float alpha;

/**
 * @brief Určuje, zda se má pro barvu fragmentu použít textura (true) nebo jen objectColor (false).
 */
uniform bool useTexture;

/**
 * @brief Pozice kamery (pozorovatele) ve světovém prostoru pro výpočet odlesků.
 */
uniform vec3 viewPos; 

/**
 * @brief Přepínač pro režim vykreslování částic. 
 * Pokud je true, fragment ignoruje osvětlení a stíny.
 */
uniform bool isParticle; 

/**
 * @brief Lesklost materiálu (ovlivňuje velikost a ostrost spekulárního odlesku).
 */
uniform float shininess;        

/**
 * @brief Intenzita zrcadlového odlesku materiálu.
 */
uniform float specularStrength; 

/**
 * @struct DirLight
 * @brief Struktura definující směrové světlo (např. slunce).
 */
struct DirLight {
    vec3 direction; ///< Směr, kterým světlo svítí.
    vec3 ambient;   ///< Ambientní složka světla.
    vec3 diffuse;   ///< Difuzní složka světla.
    vec3 specular;  ///< Spekulární složka světla.
};

/**
 * @struct PointLight
 * @brief Struktura definující bodové světlo (např. žárovka).
 */
struct PointLight {
    vec3 position;  ///< Pozice světla v prostoru.
    float constant; ///< Konstantní složka útlumu světla.
    float linear;   ///< Lineární složka útlumu světla.
    float quadratic;///< Kvadratická složka útlumu světla.
    vec3 ambient;   ///< Ambientní složka světla.
    vec3 diffuse;   ///< Difuzní složka světla.
    vec3 specular;  ///< Spekulární složka světla.
};

/**
 * @struct SpotLight
 * @brief Struktura definující reflektorové světlo (např. baterka).
 */
struct SpotLight {
    vec3 position;    ///< Pozice zdroje světla.
    vec3 direction;   ///< Směr, kterým reflektor svítí.
    float cutOff;     ///< Vnitřní úhel kužele světla (kosinus úhlu).
    float outerCutOff;///< Vnější úhel kužele světla pro plynulý přechod do tmy.
    float constant;   ///< Konstantní složka útlumu.
    float linear;     ///< Lineární složka útlumu.
    float quadratic;  ///< Kvadratická složka útlumu.
    vec3 ambient;     ///< Ambientní složka světla.
    vec3 diffuse;     ///< Difuzní složka světla.
    vec3 specular;    ///< Spekulární složka světla.
};

/**
 * @brief Maximální možný počet bodových světel ve scéně.
 */
#define MAX_POINT_LIGHTS 5 

/**
 * @brief Aktuální počet aktivních bodových světel pro daný snímek.
 */
uniform int numPointLights; 

/**
 * @brief Instance směrového světla.
 */
uniform DirLight dirLight;

/**
 * @brief Pole všech dostupných bodových světel ve scéně.
 */
uniform PointLight pointLights[MAX_POINT_LIGHTS];

/**
 * @brief Instance reflektorového světla (baterky).
 */
uniform SpotLight spotLight;

/**
 * @brief Vypočítá vliv směrového světla na barvu fragmentu.
 * @param light Struktura s parametry směrového světla.
 * @param normal Normálový vektor fragmentu.
 * @param viewDir Směrový vektor k pozorovateli.
 * @param diffColor Základní (difuzní) barva materiálu.
 * @param specColor Spekulární barva materiálu.
 * @return Výsledná barva fragmentu ovlivněná tímto světlem.
 */
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffColor, vec3 specColor);

/**
 * @brief Vypočítá vliv bodového světla na barvu fragmentu, včetně útlumu vzdáleností.
 * @param light Struktura s parametry bodového světla.
 * @param normal Normálový vektor fragmentu.
 * @param fragPos Pozice fragmentu ve světovém prostoru.
 * @param viewDir Směrový vektor k pozorovateli.
 * @param diffColor Základní (difuzní) barva materiálu.
 * @param specColor Spekulární barva materiálu.
 * @return Výsledná barva fragmentu ovlivněná tímto světlem.
 */
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffColor, vec3 specColor);

/**
 * @brief Vypočítá vliv reflektorového světla na barvu fragmentu, včetně útlumu a oříznutí kužele.
 * @param light Struktura s parametry reflektorového světla.
 * @param normal Normálový vektor fragmentu.
 * @param fragPos Pozice fragmentu ve světovém prostoru.
 * @param viewDir Směrový vektor k pozorovateli.
 * @param diffColor Základní (difuzní) barva materiálu.
 * @param specColor Spekulární barva materiálu.
 * @return Výsledná barva fragmentu ovlivněná tímto světlem.
 */
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffColor, vec3 specColor);

/**
 * @brief Hlavní funkce fragment shaderu.
 * * Zajišťuje sečtení všech světelných zdrojů aplikovaných na fragment, 
 * zpracovává případné částicové efekty a dynamickou úpravu průhlednosti.
 */
void main() {
    if (isParticle) {
        FragColor = vec4(objectColor, alpha);
        return;
    }

    vec3 diffColor = useTexture ? texture(texture1, TexCoords).rgb * objectColor : objectColor;
    vec3 specColor = vec3(specularStrength); 

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalcDirLight(dirLight, norm, viewDir, diffColor, specColor);
    
    for(int i = 0; i < numPointLights; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, diffColor, specColor);    
        
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir, diffColor, specColor);    
    
    float finalAlpha = alpha;
    if (!useTexture) {
        float brightness = dot(result, vec3(0.299, 0.587, 0.114)); 
        finalAlpha = clamp(alpha + brightness * 1.5, 0.0, 1.0); 
    }
    
    FragColor = vec4(result, finalAlpha);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffColor, vec3 specColor) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess); 
    
    vec3 ambient = light.ambient * diffColor;
    vec3 diffuse = light.diffuse * diff * diffColor;
    vec3 specular = light.specular * spec * specColor;
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffColor, vec3 specColor) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess); 
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    vec3 ambient = light.ambient * diffColor * attenuation;
    vec3 diffuse = light.diffuse * diff * diffColor * attenuation;
    vec3 specular = light.specular * spec * specColor * attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffColor, vec3 specColor) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess); 
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    vec3 ambient = light.ambient * diffColor * attenuation * intensity;
    vec3 diffuse = light.diffuse * diff * diffColor * attenuation * intensity;
    vec3 specular = light.specular * spec * specColor * attenuation * intensity;
    return (ambient + diffuse + specular);
}