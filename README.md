## cheerlights_Saberes&Fazeres
***

Explorando o [@CheerLights](https://cheerlights.com/) como meio de instrução para a programação da placa ESP32 Heltec V2. 

Primeiro um pouquinho de história!
CheerLights é um projeto de “Internet das Coisas” criado por Hans Scharler que permite que as luzes (lâmpadas especiais, fitas ou cordões de leds) de pessoas em todo o mundo sejam sincronizadas com uma cor sugerida por meio de uma mensagem pelo Twitter. Esta é um experimentação que demonstra como conectar/comandar coisas diversas (arranjos luminosos de Natal, displays, painéis ou aplicativos) por intermédio de redes sociais, no caso em questão, [Twitter](https://twitter.com/).

Experimente, você mesmo, enviando um tweet. Simplesmente digite: **@CheerLights red now** <enter>

Essa mensagem será processada e causará uma reação em cadeia, fazendo com que todos os dispositivos "CheerLights" mudem de cor para vermelho, inclusive o painel de leds ao lado da minha mesa. As cores podem ser: *red, pink, green, blue, cyan, white, warmwhite, oldlace, purple, magenta, yellow ou orange*. Escolha uma cor e mande bala, ou melhor, pode *tweetar* à vontade.

![painel](https://github.com/Mario-Camara/cheerlights_Saberes-Fazeres/blob/main/cheerlights_saberes-fazeres.jpg?raw=true)

***

Muito bem e para que serve isto? Por qual motivo terei interesse em mudar a cor da [árvore de luzes](https://cheerlights.com/live/) do vizinho, mesmo sabendo que este "vizinho invisível" poderá estar do outro lado do planeta? E daí ...

Bem, o CheerLights é capaz de proporcionar um ambiente bastante fértil para explorar um série de recursos de programação, a depender de sua imaginação e criatividade. Uma vez se divertido e brincado, sem  compromisso, com o CheerLights; fica mais simples e fácil transpor o conhecimento adquirido para uma aplicação prática. **Inspirá-lo**, será o objetivo do *sketch* apresentado em anexo, o qual abordará:
***
- [x] v.1: vamos lá, sei que você, assim como eu, não se contentará com a monotonia de ficar olhando uma única cor (painel monocromático). Portanto, façamos dois tipos de registros históricos: (i) um visual (usando o cordão de leds) com os 48 últimos registros de cores amostrados (discretizados) a cada 19 segundos; e (ii) transferir dados para gerar um gráfico com a cor e o respectivo intervalo de permanência (tempo de cronômetro) da cor inalterada;  
- [x] v.1.1: utilização do FastLED v.3.3.3 e display (OLED) do ESP32 com a indicação da cor e tempo de exposição (cronômetro) da atual cor;
- [x] v.1.2: captação de um idDigital, tal como uma "assinatura em livro de visita" daqueles que instalarem e rodarem o sketch em suas placas. A intenção não é de identificar, tão somente registrar uma nova instalação do sketch, como se fosse, uma rubrica;
- [x] v.1.3: [ThingSpeak](https://thingspeak.com/channels/1270120) recebe os dados (idDigital, color e cronômetro) e apresenta num gráfico temporal; e
- [ ] v.2: dashboard WEB (Qnt de idDigitais, corCurrent, chronometer e gráfico de histórico - TS). Esta parece-me a tarefa mais complicada e que demandará maior tempo. Alguém se habilita a ajudar? :fist_right: :fist_left:
***
Vou limitar esta tarefa por aqui, pois já é invenção suficiente para carrear um bom trabalho (:coffee:). **Fique de olho no [CheerLights!](https://cheerlights.com/)**
