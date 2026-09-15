package org.example.newpl.demo

import javafx.animation.KeyFrame
import javafx.animation.Timeline
import javafx.application.Application
import javafx.geometry.Insets
import javafx.geometry.Pos
import javafx.scene.Scene
import javafx.scene.canvas.Canvas
import javafx.scene.canvas.GraphicsContext
import javafx.scene.control.*
import javafx.scene.layout.BorderPane
import javafx.scene.layout.HBox
import javafx.scene.layout.VBox
import javafx.scene.paint.Color
import javafx.scene.text.Font
import javafx.stage.Stage
import javafx.util.Duration
import java.io.File
import kotlin.math.cos
import kotlin.math.sin
import javafx.event.EventHandler

fun main() {
    Application.launch(TSPViewerApp::class.java)
}

data class Step(
    val phase: String,
    val curCost: Double,
    val lb: Double,
    val best: Double,
    val curPath: List<Int>,
    val bestPath: List<Int>,
    val mstEdges: Set<Pair<Int, Int>>,
    val message: String
)

data class GraphData(
    val n: Int,
    val matrix: Array<DoubleArray>,
    val steps: List<Step>
)

class TSPViewerApp : Application() {
    private lateinit var data: GraphData
    private var currentIndex = 0
    private var isPlaying = false
    private val timer = Timeline()
    private var speed: Long = 800
    private lateinit var gc: GraphicsContext

    private lateinit var vx: DoubleArray
    private lateinit var vy: DoubleArray

    private val lblStep = Label("Шаг: 0 / 0")
    private val lblPhase = Label("Фаза: —")
    private val lblCurrentCost = Label("Текущая стоимость: —")
    private val lblLowerBound = Label("Нижняя оценка: —")
    private val lblBestCost = Label("Лучшая стоимость: —")
    private val lblCurrentPath = Label("Текущий путь: —")
    private val lblBestPath = Label("Лучший маршрут: —")
    private val lblMessage = Label("Сообщение: ")

    override fun start(stage: Stage) {
        val file = File("out.txt")
        if (!file.exists()) {
            showError("Файл out.txt не найден рядом с приложением")
            return
        }
        data = parseFile(file)

        // раскладка вершин по кругу
        val cx = 400.0
        val cy = 350.0
        val r = 250.0
        vx = DoubleArray(data.n)
        vy = DoubleArray(data.n)
        for (i in 0 until data.n) {
            val angle = 2 * Math.PI * i / data.n - Math.PI / 2
            vx[i] = cx + r * cos(angle)
            vy[i] = cy + r * sin(angle)
        }

        val canvas = Canvas(800.0, 700.0)
        gc = canvas.graphicsContext2D

        // кнопки 
        val btnPlay = Button("▶")
        val btnPause = Button("⏸")
        val btnPrev = Button("↩")
        val btnNext = Button("↪")

        listOf(btnPlay, btnPause, btnPrev, btnNext).forEach { b ->
            b.style = """
                -fx-min-width: 60px; -fx-min-height: 60px;
                -fx-max-width: 60px; -fx-max-height: 60px;
                -fx-pref-width: 60px; -fx-pref-height: 60px;
                -fx-font-size: 24px;
                -fx-background-color: #f0f0f0;
                -fx-cursor: hand;
            """.trimIndent()
            b.isFocusTraversable = false
        }

        val slider = Slider(200.0, 3000.0, 800.0)
        slider.prefWidth = 150.0
        slider.isShowTickLabels = true
        slider.isShowTickMarks = true
        slider.majorTickUnit = 500.0
        slider.valueProperty().addListener { _, _, v ->
            speed = v.toLong()
            if (isPlaying) { pause(); play() }
        }

        btnPlay.setOnAction { play() }
        btnPause.setOnAction { pause() }
        btnPrev.setOnAction { stepBackward() }
        btnNext.setOnAction { stepForward() }

        // панели 
        val infoPanel = VBox(8.0)
        infoPanel.padding = Insets(15.0)
        infoPanel.style = "-fx-background-color: #f4f4f4; -fx-border-color: #cccccc;"
        lblMessage.isWrapText = true
        lblCurrentPath.isWrapText = true
        lblBestPath.isWrapText = true
        infoPanel.children.addAll(
            lblStep, lblPhase, Separator(),
            lblCurrentCost, lblLowerBound, lblBestCost, Separator(),
            lblCurrentPath, lblBestPath, Separator(),
            lblMessage
        )
        infoPanel.prefWidth = 320.0
        infoPanel.minWidth = 320.0

        val controlBox = HBox(10.0)
        controlBox.alignment = Pos.CENTER
        controlBox.children.addAll(btnPlay, btnPause, btnPrev, btnNext, slider)
        controlBox.style = "-fx-background-color: #f0f0f0; -fx-border-color: #cccccc; -fx-border-width: 1px 0 0 0;"
        controlBox.prefHeight = 80.0
        controlBox.minHeight = 80.0
        controlBox.maxHeight = 80.0

        val root = BorderPane()
        root.center = canvas
        root.bottom = controlBox
        root.right = infoPanel

        stage.title = "Визуализация задачи коммивояжёра"
        stage.scene = Scene(root, 1200.0, 800.0)
        stage.show()

        updateUI()
    }

    private fun play() {
        if (currentIndex >= data.steps.size - 1) return
        isPlaying = true
        timer.stop()
        timer.keyFrames.clear()
        timer.keyFrames.add(KeyFrame(Duration.millis(speed.toDouble()), EventHandler { stepForward() }))
        timer.cycleCount = Timeline.INDEFINITE
        timer.play()
    }

    private fun pause() {
        isPlaying = false
        timer.stop()
    }

    private fun stepForward() {
        if (currentIndex < data.steps.size - 1) {
            currentIndex++
            updateUI()
        } else pause()
    }

    private fun stepBackward() {
        if (currentIndex > 0) {
            currentIndex--
            updateUI()
        }
    }

    private fun updateUI() {
        if (data.steps.isEmpty()) return
        val s = data.steps[currentIndex]
        lblStep.text = "Шаг: ${currentIndex + 1} / ${data.steps.size}"
        lblPhase.text = "Фаза: ${phaseName(s.phase)}"
        lblCurrentCost.text = "Текущая стоимость: ${fmt(s.curCost)}"
        lblLowerBound.text = "Нижняя оценка: ${if (s.lb < 0) "∞" else fmt(s.lb)}"
        lblBestCost.text = "Лучшая стоимость: ${if (s.best < 0) "—" else fmt(s.best)}"
        lblCurrentPath.text = "Текущий путь: " + (s.curPath.joinToString(" → ").ifEmpty { "—" })
        lblBestPath.text = "Лучший маршрут: " + (s.bestPath.joinToString(" → ").ifEmpty { "—" })
        lblMessage.text = "Сообщение: ${s.message}"
        draw()
    }

    private fun draw() {
        gc.fill = Color.WHITE
        gc.fillRect(0.0, 0.0, gc.canvas.width, gc.canvas.height)
        if (data.steps.isEmpty()) return

        val s = data.steps[currentIndex]
        val curEdges = pathToEdges(s.curPath, false)
        val bestEdges = pathToEdges(s.bestPath, true)

        // рёбра
        for (i in 0 until data.n) {
            for (j in i + 1 until data.n) {
                val w = data.matrix[i][j]
                if (w < 0 || w >= 1e17) continue
                val key = i to j
                val inBest = key in bestEdges
                val inCur = key in curEdges
                val inMst = key in s.mstEdges

                gc.lineWidth = when {
                    inBest -> 5.0
                    inCur -> 4.0
                    inMst -> 3.0
                    else -> 1.0
                }
                val showMst = s.phase == "MST_BUILD" || s.phase == "MST_WALK"

                gc.stroke = when {
                    s.phase == "PRUNE" && inCur -> Color.RED
                    inBest -> Color.BLUE
                    inCur -> Color.ORANGE
                    showMst && inMst -> Color.GREEN
                    else -> Color.LIGHTGRAY
                }

                if (s.phase == "PRUNE" && inCur) gc.setLineDashes(8.0, 6.0)
                else gc.setLineDashes()

                gc.strokeLine(vx[i], vy[i], vx[j], vy[j])

                if (inCur || inBest || inMst) {
                    gc.fill = Color.DARKGRAY
                    gc.font = Font(12.0)
                    val mx = (vx[i] + vx[j]) / 2
                    val my = (vy[i] + vy[j]) / 2
                    gc.fillText(fmt(w), mx + 4, my - 4)
                }
            }
        }
        gc.setLineDashes()

        // вершины
        val curSet = s.curPath.toSet()
        val bestSet = s.bestPath.toSet()
        for (i in 0 until data.n) {
            val r = 22.0
            gc.fill = when {
                i in bestSet -> Color.LIGHTBLUE
                i in curSet -> Color.ORANGE
                else -> Color.WHITE
            }
            gc.fillOval(vx[i] - r, vy[i] - r, r * 2, r * 2)

            gc.stroke = when {
                i == s.curPath.lastOrNull() -> Color.DARKORANGE
                i in bestSet -> Color.BLUE
                else -> Color.BLACK
            }
            gc.lineWidth = if (i == s.curPath.lastOrNull()) 3.0 else 1.5
            gc.strokeOval(vx[i] - r, vy[i] - r, r * 2, r * 2)

            gc.fill = Color.BLACK
            gc.font = Font(14.0)
            gc.fillText("V${i + 1}", vx[i] - 12, vy[i] + 5)
        }
    }

    private fun pathToEdges(path: List<Int>, close: Boolean): Set<Pair<Int, Int>> {
        val res = mutableSetOf<Pair<Int, Int>>()
        for (i in 0 until path.size - 1) {
            val a = path[i]; val b = path[i + 1]
            res.add(minOf(a, b) to maxOf(a, b))
        }
        if (close && path.size >= 2) {
            val a = path.last(); val b = path.first()
            res.add(minOf(a, b) to maxOf(a, b))
        }
        return res
    }

    private fun phaseName(p: String) = when (p) {
        "MST_BUILD" -> "Построение MST"
        "MST_WALK" -> "Обход MST"
        "BRANCH" -> "Ветвление"
        "PRUNE" -> "Отсечение"
        "IMPROVE" -> "Улучшение"
        "DONE" -> "Завершено"
        else -> p
    }

    private fun fmt(v: Double) = "%.2f".format(v)

    private fun parseFile(file: File): GraphData {
        val lines = file.readLines().filter { it.isNotEmpty() }
        var idx = 0
        val n = lines[idx++].trim().toInt()
        val matrix = Array(n) { DoubleArray(n) }
        for (i in 0 until n) {
            val parts = lines[idx++].trim().split("\\s+".toRegex())
            for (j in 0 until n) matrix[i][j] = parts[j].toDouble()
        }
        val count = lines[idx++].trim().toInt()
        val steps = mutableListOf<Step>()
        for (k in 0 until count) {
            val parts = lines[idx++].split("\t")
            val phase = parts[0]
            val curCost = parts[1].toDouble()
            val lb = parts[2].toDouble()
            val best = parts[3].toDouble()
            val curPath = if (parts[4].isEmpty()) emptyList() else parts[4].split(",").map { it.toInt() }
            val bestPath = if (parts[5].isEmpty()) emptyList() else parts[5].split(",").map { it.toInt() }
            val mstEdges = if (parts[6].isEmpty()) emptySet() else parts[6].split(",").map {
                val p = it.split("-")
                p[0].toInt() to p[1].toInt()
            }.toSet()
            val message = if (parts.size > 7) parts[7] else ""
            steps.add(Step(phase, curCost, lb, best, curPath, bestPath, mstEdges, message))
        }
        return GraphData(n, matrix, steps)
    }

    private fun showError(msg: String) {
        Alert(Alert.AlertType.ERROR).apply {
            title = "Ошибка"
            headerText = null
            contentText = msg
            showAndWait()
        }
    }
}